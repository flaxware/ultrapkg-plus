#include <gtkmm.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string execCapture(const std::string& command) {
    std::array<char, 4096> buffer{};
    std::string result;
    auto* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return {};
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }

    pclose(pipe);
    return result;
}

std::string shellQuote(const std::string& value) {
    std::string out;
    out.reserve(value.size() + 2);
    out.push_back('\'');
    for (char c : value) {
        if (c == '\'') {
            out += "'\\''";
        } else {
            out.push_back(c);
        }
    }
    out.push_back('\'');
    return out;
}

} // namespace

class MainWindow final : public Gtk::Window {
public:
    MainWindow() {
        set_title("Ultrapkg Plus");
        set_default_size(980, 660);
        set_resizable(true);
        set_name("ultrapkg-window");

        auto* css = Gtk::CssProvider::create();
        css->load_from_data(
            "window#ultrapkg-window { background: linear-gradient(180deg, #0b1020 0%, #0d1220 45%, #111827 100%); }"
            "box#hero { background: rgba(255,255,255,0.03); border: 1px solid rgba(255,255,255,0.08); border-radius: 18px; }"
            "button { background: #111827; color: #f8fafc; border-radius: 12px; padding: 10px 16px; }"
            "button:hover { background: #1f2937; }"
            "button#primary { background: linear-gradient(180deg, #7c3aed 0%, #8b5cf6 100%); border: none; }"
            "label#title { color: #f8fafc; font-size: 28px; font-weight: 700; }"
            "label#subtitle { color: #cbd5e1; font-size: 14px; }"
            "label#meta { color: #e2e8f0; font-size: 15px; }"
            "box#dropzone { background: rgba(124,58,237,0.09); border: 1px dashed rgba(167,139,250,0.9); border-radius: 20px; }"
        );

        auto* screen = get_screen();
        if (screen) {
            auto context = get_style_context();
            context->add_provider_for_screen(*screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }

        root_.set_orientation(Gtk::Orientation::VERTICAL);
        root_.set_spacing(18);
        set_child(root_);

        header_.set_title_widget(title_label_);
        header_.set_show_title_buttons(true);
        header_.set_hexpand(true);
        title_label_.set_text("Ultrapkg Plus");
        title_label_.set_name("title");
        header_.set_title_widget(title_label_);

        subtitle_.set_text("Premium package acquisition for Flax system");
        subtitle_.set_name("subtitle");

        hero_.set_name("hero");
        hero_.set_orientation(Gtk::Orientation::VERTICAL);
        hero_.set_spacing(16);
        hero_.set_margin_top(18);
        hero_.set_margin_start(18);
        hero_.set_margin_end(18);
        hero_.set_margin_bottom(18);

        info_label_.set_text("Drop a .deb package here or choose one from disk.");
        info_label_.set_name("meta");
        info_label_.set_halign(Gtk::Align::CENTER);

        dropzone_.set_name("dropzone");
        dropzone_.set_margin_start(20);
        dropzone_.set_margin_end(20);
        dropzone_.set_margin_top(10);
        dropzone_.set_margin_bottom(10);
        dropzone_.set_valign(Gtk::Align::CENTER);
        dropzone_.set_hexpand(true);
        dropzone_.set_vexpand(true);
        dropzone_.set_size_request(-1, 180);
        dropzone_.set_orientation(Gtk::Orientation::VERTICAL);
        dropzone_.set_spacing(8);
        dropzone_.set_halign(Gtk::Align::CENTER);

        drop_icon_.set_text("⬇");
        drop_icon_.set_margin_top(18);
        drop_icon_.set_margin_bottom(8);
        drop_icon_.set_halign(Gtk::Align::CENTER);
        drop_icon_.set_name("meta");
        drop_icon_.override_font(Pango::FontDescription("Sans Bold 36"));

        drop_text_.set_text("Drop package here");
        drop_text_.set_halign(Gtk::Align::CENTER);
        drop_text_.set_name("meta");

        file_name_.set_text("No file selected");
        file_name_.set_halign(Gtk::Align::CENTER);
        file_name_.set_name("meta");

        actions_.set_halign(Gtk::Align::CENTER);
        actions_.set_spacing(12);
        actions_.set_margin_bottom(16);

        select_button_.set_label("Select .deb");
        install_button_.set_label("Install");
        install_button_.set_name("primary");

        select_button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onSelectFile));
        install_button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onInstallSelected));

        actions_.append(select_button_);
        actions_.append(install_button_);

        dropzone_.append(drop_icon_);
        dropzone_.append(drop_text_);
        dropzone_.append(file_name_);
        dropzone_.append(actions_);

        hero_.append(info_label_);
        hero_.append(dropzone_);

        root_.append(header_);
        root_.append(subtitle_);
        root_.append(hero_);
    }

    void setSelectedFile(const std::filesystem::path& path) {
        selected_file_ = path;
        file_name_.set_text(selected_file_.string());
    }

private:
    void onSelectFile() {
        auto dialog = Gtk::FileChooserNative::create("Select package", *this, Gtk::FileChooserAction::OPEN, "Select", "Cancel");
        auto filter = Gtk::FileFilter::create();
        filter->set_name("Debian packages");
        filter->add_pattern("*.deb");
        dialog->add_filter(filter);

        if (dialog->run() == Gtk::ResponseType::ACCEPT) {
            setSelectedFile(dialog->get_file()->get_path());
        }
    }

    void onInstallSelected() {
        if (selected_file_.empty()) {
            std::cerr << "No package selected\n";
            return;
        }

        const std::string command = "ultrapkg install " + shellQuote(selected_file_.string());
        const int rc = std::system(command.c_str());
        if (rc == 0) {
            std::cout << "Install request sent successfully\n";
        } else {
            std::cerr << "Install failed.\n";
        }
    }

    Gtk::Box root_{Gtk::Orientation::VERTICAL};
    Gtk::HeaderBar header_;
    Gtk::Label title_label_{"Ultrapkg Plus"};
    Gtk::Label subtitle_{"Premium package acquisition for Flax system"};
    Gtk::Box hero_{Gtk::Orientation::VERTICAL};
    Gtk::Label info_label_;
    Gtk::Box dropzone_{Gtk::Orientation::VERTICAL};
    Gtk::Label drop_icon_;
    Gtk::Label drop_text_;
    Gtk::Label file_name_;
    Gtk::Box actions_{Gtk::Orientation::HORIZONTAL};
    Gtk::Button select_button_;
    Gtk::Button install_button_;
    std::filesystem::path selected_file_;
};

int main(int argc, char** argv) {
    auto app = Gtk::Application::create("org.flax.ultrapkgplus");
    auto window = Gtk::make_managed<MainWindow>();
    return app->run(*window, argc, argv);
}
