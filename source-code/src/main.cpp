#include <gtkmm.h>

#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr const char* kAppVersion = "v0.1.0";

struct PackageRecord {
    std::string id;
    std::string name;
    std::string summary;
    std::string description;
    std::string source;
    std::string version;
    std::string icon_name;
    std::string package_name;
    std::string flatpak_id;
    bool installed = false;
    bool update_available = false;
};

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string shellQuote(const std::string& value) {
    std::string out;
    out.reserve(value.size() + 2);
    out.push_back('\'');
    for (const char ch : value) {
        if (ch == '\'') {
            out += "'\\''";
        } else {
            out.push_back(ch);
        }
    }
    out.push_back('\'');
    return out;
}

std::string packageStatusText(const PackageRecord& package) {
    if (package.installed) {
        return package.update_available ? "Update ready" : "Installed";
    }
    return package.update_available ? "Update ready" : "Available";
}

std::string sourceDisplayName(const std::string& source) {
    if (source == "apt") return "APT";
    if (source == "flatpak") return "Flatpak";
    if (source == "appimage") return "AppImage";
    return "System";
}

std::vector<PackageRecord> makeDemoPackages() {
    return {
        {"firefox", "Firefox", "Fast and private browsing", "Firefox offers a fast, secure, and familiar browser experience that stays lightweight on Linux desktops.", "apt", "140.0", "firefox", "firefox", "", true, false},
        {"vlc", "VLC", "Media player and codec suite", "VLC plays media across many formats without dragging in a heavy desktop stack.", "apt", "3.0.20", "video-x-generic", "vlc", "", false, false},
        {"thunar", "Thunar", "XFCE file manager", "Thunar keeps file operations quick, consistent, and highly compatible with XFCE desktops.", "apt", "4.18.13", "folder", "thunar", "", true, false},
        {"git", "Git", "Version control", "Git provides the reliable foundation for source control, patches, and lightweight system maintenance.", "apt", "2.45.0", "git", "git", "", false, false},
        {"steam", "Steam", "Gaming client", "Steam brings Linux gaming to the desktop with a polished launcher and storefront.", "flatpak", "2.0.0", "steam", "steam", "com.valvesoftware.Steam", false, true},
        {"discord", "Discord", "Messaging", "Discord keeps communities connected with voice, chat, and low-overhead collaboration.", "flatpak", "0.29.0", "discord", "discord", "com.discordapp.Discord", false, false},
        {"signal", "Signal Desktop", "Encrypted messaging", "Signal provides strong end-to-end encrypted messaging for secure daily communication.", "flatpak", "7.5.0", "signal-desktop", "signal-desktop", "org.signal.Signal", false, false},
        {"obsidian", "Obsidian", "Knowledge workspace", "Obsidian gives a focused environment for notes, references, and long-form writing.", "appimage", "1.7.16", "accessories-text-editor", "obsidian", "md.obsidian.Obsidian", false, false},
        {"krita", "Krita", "Digital painting", "Krita keeps artistic workflows responsive and polished, without unnecessary desktop weight.", "flatpak", "5.2.0", "krita", "krita", "org.krita.Krita", false, false},
        {"gdebi", "GDebi", "Debian package installer", "GDebi is the classic, minimal way to install local .deb files with dependency handling and clean UX.", "apt", "0.9", "package-x-generic", "gdebi", "", false, false},
        {"appimagelauncher", "AppImageLauncher", "AppImage handling", "AppImageLauncher makes AppImage workflows smoother for desktop users who prefer portable apps.", "apt", "2.2.0", "application-x-executable", "appimagelauncher", "", false, false}
    };
}

std::string packageActionCommand(const PackageRecord& package) {
    if (package.source == "apt") {
        return "ultrapkg install " + shellQuote(package.package_name.empty() ? package.name : package.package_name);
    }
    if (package.source == "flatpak") {
        return "flatpak install -y " + shellQuote(package.flatpak_id.empty() ? package.package_name : package.flatpak_id);
    }
    if (package.source == "appimage") {
        return "ultrapkg install-appimage " + shellQuote(package.package_name.empty() ? package.name : package.package_name);
    }
    return "ultrapkg install " + shellQuote(package.package_name.empty() ? package.name : package.package_name);
}

std::string packageOpenCommand(const std::filesystem::path& path) {
    const std::string ext = lower(path.extension().string());
    if (ext == ".deb") {
        return "if command -v gdebi >/dev/null 2>&1; then gdebi --non-interactive " + shellQuote(path.string()) + "; else ultrapkg install " + shellQuote(path.string()) + "; fi";
    }
    if (ext == ".appimage") {
        return "chmod +x " + shellQuote(path.string()) + " && " + shellQuote(path.string());
    }
    if (ext == ".flatpakref") {
        return "flatpak install --user " + shellQuote(path.string());
    }
    if (ext == ".flatpak") {
        return "flatpak install --user " + shellQuote(path.string());
    }
    return "ultrapkg install-file " + shellQuote(path.string());
}

} // namespace

class PackageCard final : public Gtk::Box {
public:
    PackageCard(const PackageRecord& package,
                const std::function<void(const PackageRecord&)>& on_details,
                const std::function<void(const PackageRecord&)>& on_install)
        : Gtk::Box(Gtk::Orientation::VERTICAL), package_(package), on_details_(on_details), on_install_(on_install) {
        set_name("package-card");
        set_margin_top(6);
        set_margin_bottom(6);
        set_margin_start(6);
        set_margin_end(6);
        set_spacing(8);
        set_hexpand(true);
        set_size_request(250, 190);

        auto* header = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
        header->set_spacing(10);
        header->set_hexpand(true);

        auto* icon = Gtk::make_managed<Gtk::Image>();
        icon->set_from_icon_name(package.icon_name.empty() ? "package-x-generic" : package.icon_name);
        icon->set_pixel_size(34);
        header->append(*icon);

        auto* title_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
        title_box->set_hexpand(true);

        auto* title = Gtk::make_managed<Gtk::Label>();
        title->set_markup("<b>" + package.name + "</b>");
        title->set_xalign(0.0f);
        title_box->append(*title);

        auto* meta = Gtk::make_managed<Gtk::Label>(sourceDisplayName(package.source) + " • " + package.version);
        meta->set_name("small-meta");
        meta->set_xalign(0.0f);
        title_box->append(*meta);

        header->append(*title_box);
        append(*header);

        auto* summary = Gtk::make_managed<Gtk::Label>(package.summary);
        summary->set_wrap(true);
        summary->set_xalign(0.0f);
        summary->set_max_width_chars(26);
        append(*summary);

        auto* footer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
        footer->set_spacing(8);
        footer->set_hexpand(true);

        auto* status = Gtk::make_managed<Gtk::Label>(packageStatusText(package));
        status->set_name(package.installed ? "status-good" : "status-muted");
        footer->append(*status);

        auto* details_button = Gtk::make_managed<Gtk::Button>("Details");
        details_button->signal_clicked().connect([this] { on_details_(package_); });

        auto* install_button = Gtk::make_managed<Gtk::Button>(package.installed ? "Update" : "Install");
        install_button->set_name("action-button");
        install_button->signal_clicked().connect([this] { on_install_(package_); });

        footer->append(*details_button);
        footer->append(*install_button);
        append(*footer);
    }

private:
    PackageRecord package_;
    std::function<void(const PackageRecord&)> on_details_;
    std::function<void(const PackageRecord&)> on_install_;
};

class MainWindow final : public Gtk::Window {
public:
    MainWindow() {
        set_title("Ultrapkg Plus");
        set_default_size(1280, 820);
        set_resizable(true);
        set_name("ultrapkg-window");
        set_icon_name("ultrapkg-plus");

        const std::string css = R"(
            window#ultrapkg-window {
                background: linear-gradient(180deg, #0b1020 0%, #111827 100%);
                color: #e2e8f0;
            }
            box#sidebar {
                background: rgba(15, 23, 42, 0.65);
                border: 1px solid rgba(148, 163, 184, 0.15);
                border-radius: 18px;
            }
            button#sidebar-button {
                background: rgba(148, 163, 184, 0.08);
                color: #f8fafc;
                border: 1px solid rgba(148, 163, 184, 0.14);
                border-radius: 12px;
                padding: 10px 12px;
            }
            button#sidebar-button:hover {
                background: rgba(168, 85, 247, 0.18);
            }
            button#sidebar-button.active {
                background: linear-gradient(180deg, rgba(139, 92, 246, 0.34), rgba(91, 33, 182, 0.4));
                border-color: rgba(216, 180, 254, 0.5);
            }
            box#package-card {
                background: rgba(15, 23, 42, 0.52);
                border: 1px solid rgba(148, 163, 184, 0.12);
                border-radius: 16px;
            }
            label#small-meta {
                color: #94a3b8;
                font-size: 11px;
            }
            label#status-good {
                color: #86efac;
            }
            label#status-muted {
                color: #cbd5e1;
            }
            button#action-button {
                background: linear-gradient(180deg, #7c3aed 0%, #8b5cf6 100%);
                color: white;
                border: none;
                border-radius: 10px;
            }
            button#action-button:hover {
                background: linear-gradient(180deg, #8b5cf6 0%, #a78bfa 100%);
            }
            entry#search-entry {
                background: rgba(15, 23, 42, 0.7);
                border: 1px solid rgba(148, 163, 184, 0.23);
                border-radius: 12px;
                color: #f8fafc;
            }
            button#primary {
                background: linear-gradient(180deg, #7c3aed 0%, #8b5cf6 100%);
                color: #fff;
                border: none;
                border-radius: 12px;
                padding: 10px 16px;
            }
            button#primary:hover {
                background: linear-gradient(180deg, #8b5cf6 0%, #a78bfa 100%);
            }
            button#secondary {
                background: rgba(148, 163, 184, 0.08);
                color: #f8fafc;
                border: 1px solid rgba(148, 163, 184, 0.18);
                border-radius: 12px;
                padding: 10px 16px;
            }
            button#secondary:hover {
                background: rgba(148, 163, 184, 0.14);
            }
            box#detail-pane {
                background: rgba(15, 23, 42, 0.42);
                border: 1px solid rgba(148, 163, 184, 0.12);
                border-radius: 18px;
            }
            label#details-title {
                font-size: 28px;
                font-weight: 700;
            }
            label#details-body {
                color: #dbeafe;
            }
            label#details-source {
                color: #c4b5fd;
            }
            statusbar {
                background: rgba(15, 23, 42, 0.38);
                color: #cbd5e1;
            }
        )";

        auto css_provider = Gtk::CssProvider::create();
        css_provider->load_from_string(css);
        auto* display = Gdk::Display::get_default();
        if (display) {
            Gtk::StyleContext::add_provider_for_display(*display, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        }

        root_.set_orientation(Gtk::Orientation::VERTICAL);
        root_.set_spacing(10);
        set_child(root_);

        header_.set_title_widget(title_widget_);
        header_.set_show_title_buttons(true);
        header_.set_hexpand(true);
        title_widget_.set_text("Ultrapkg Plus");
        title_widget_.set_name("details-title");

        toolbar_.set_spacing(12);
        toolbar_.set_margin_start(18);
        toolbar_.set_margin_end(18);
        toolbar_.set_margin_top(8);
        toolbar_.set_margin_bottom(4);

        search_entry_.set_name("search-entry");
        search_entry_.set_placeholder_text("Search packages, sources, and apps…");
        search_entry_.set_hexpand(true);
        search_entry_.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::renderPackages));

        refresh_button_.set_label("Refresh");
        refresh_button_.set_name("secondary");
        refresh_button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::refreshData));

        local_file_button_.set_label("Open package file");
        local_file_button_.set_name("primary");
        local_file_button_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onSelectLocalFile));

        toolbar_.append(search_entry_);
        toolbar_.append(refresh_button_);
        toolbar_.append(local_file_button_);

        main_shell_.set_orientation(Gtk::Orientation::HORIZONTAL);
        main_shell_.set_spacing(18);
        main_shell_.set_margin_start(18);
        main_shell_.set_margin_end(18);
        main_shell_.set_margin_bottom(8);

        sidebar_.set_name("sidebar");
        sidebar_.set_orientation(Gtk::Orientation::VERTICAL);
        sidebar_.set_spacing(10);
        sidebar_.set_margin_top(10);
        sidebar_.set_margin_bottom(10);
        sidebar_.set_margin_start(10);
        sidebar_.set_margin_end(10);
        sidebar_.set_size_request(220, -1);

        auto addSidebarButton = [this](const std::string& id, const std::string& label) {
            auto* button = Gtk::make_managed<Gtk::Button>(label);
            button->set_name("sidebar-button");
            button->set_halign(Gtk::Align::FILL);
            button->signal_clicked().connect([this, id] { setCategory(id); });
            sidebar_.append(*button);
            sidebar_buttons_[id] = button;
        };

        addSidebarButton("overview", "Overview");
        addSidebarButton("installed", "Installed");
        addSidebarButton("updates", "Updates");
        addSidebarButton("apt", "APT");
        addSidebarButton("flatpak", "Flatpak");
        addSidebarButton("appimage", "AppImages");
        setCategory("overview");

        content_area_.set_orientation(Gtk::Orientation::HORIZONTAL);
        content_area_.set_spacing(18);
        content_area_.set_hexpand(true);
        content_area_.set_vexpand(true);

        packages_layout_.set_orientation(Gtk::Orientation::VERTICAL);
        packages_layout_.set_spacing(8);
        packages_layout_.set_vexpand(true);

        store_header_.set_text("Discover packages");
        store_header_.set_xalign(0.0f);
        store_header_.set_name("details-title");
        packages_layout_.append(store_header_);

        packages_flow_.set_hexpand(true);
        packages_flow_.set_vexpand(true);
        packages_flow_.set_selection_mode(Gtk::SelectionMode::NONE);

        package_scroll_.set_hexpand(true);
        package_scroll_.set_vexpand(true);
        package_scroll_.set_child(packages_flow_);
        packages_layout_.append(package_scroll_);

        detail_pane_.set_name("detail-pane");
        detail_pane_.set_orientation(Gtk::Orientation::VERTICAL);
        detail_pane_.set_spacing(14);
        detail_pane_.set_margin_top(10);
        detail_pane_.set_margin_bottom(10);
        detail_pane_.set_size_request(360, -1);

        detail_name_.set_name("details-title");
        detail_name_.set_xalign(0.0f);
        detail_name_.set_wrap(true);

        detail_source_.set_name("details-source");
        detail_source_.set_xalign(0.0f);

        detail_summary_.set_name("details-body");
        detail_summary_.set_xalign(0.0f);
        detail_summary_.set_wrap(true);

        detail_desc_.set_name("details-body");
        detail_desc_.set_xalign(0.0f);
        detail_desc_.set_wrap(true);

        detail_buttons_.set_spacing(10);
        detail_install_.set_name("primary");
        detail_install_.signal_clicked().connect([this] {
            if (!selected_package_.name.empty()) {
                installPackage(selected_package_);
            }
        });

        detail_file_.set_name("secondary");
        detail_file_.set_label("Open local file");
        detail_file_.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::onSelectLocalFile));

        detail_buttons_.append(detail_install_);
        detail_buttons_.append(detail_file_);

        detail_pane_.append(detail_name_);
        detail_pane_.append(detail_source_);
        detail_pane_.append(detail_summary_);
        detail_pane_.append(detail_desc_);
        detail_pane_.append(detail_buttons_);

        content_area_.append(packages_layout_);
        content_area_.append(detail_pane_);

        main_shell_.append(sidebar_);
        main_shell_.append(content_area_);

        status_label_.set_name("details-body");
        status_label_.set_margin_start(18);
        status_label_.set_margin_end(18);
        status_label_.set_margin_bottom(12);
        status_label_.set_wrap(true);

        root_.append(header_);
        root_.append(toolbar_);
        root_.append(main_shell_);
        root_.append(status_label_);

        packages_ = makeDemoPackages();
        refreshData();
    }

    void openPackageFile(const std::filesystem::path& path) {
        if (path.empty()) {
            return;
        }
        const std::string ext = lower(path.extension().string());
        if (ext == ".deb" || ext == ".appimage" || ext == ".flatpakref" || ext == ".flatpak") {
            installFile(path);
            return;
        }
        status_label_.set_text("Opened: " + path.string());
    }

private:
    void setCategory(const std::string& id) {
        current_category_ = id;
        for (const auto& [key, widget] : sidebar_buttons_) {
            widget->set_name(key == id ? "sidebar-button active" : "sidebar-button");
        }
        renderPackages();
    }

    void refreshData() {
        packages_ = makeDemoPackages();
        renderPackages();
        status_label_.set_text("Package catalogue refreshed. Ready for APT, Flatpak, and AppImage package sources.");
    }

    std::vector<PackageRecord> filteredPackages() const {
        std::vector<PackageRecord> out;
        const std::string query = lower(search_entry_.get_text());

        for (const auto& package : packages_) {
            bool include = true;
            if (current_category_ == "installed") {
                include = package.installed;
            } else if (current_category_ == "updates") {
                include = package.update_available;
            } else if (current_category_ == "apt") {
                include = package.source == "apt";
            } else if (current_category_ == "flatpak") {
                include = package.source == "flatpak";
            } else if (current_category_ == "appimage") {
                include = package.source == "appimage";
            }

            if (!include) {
                continue;
            }

            if (!query.empty()) {
                const std::string haystack = lower(package.name + " " + package.summary + " " + package.description + " " + package.source + " " + package.version);
                if (haystack.find(query) == std::string::npos) {
                    continue;
                }
            }

            out.push_back(package);
        }

        return out;
    }

    void renderPackages() {
        while (auto* child = packages_flow_.get_first_child()) {
            packages_flow_.remove(*child);
        }

        const auto filtered = filteredPackages();
        if (filtered.empty()) {
            auto* empty = Gtk::make_managed<Gtk::Label>("No packages match this view.");
            empty->set_xalign(0.0f);
            packages_flow_.append(*empty);
            return;
        }

        for (const auto& package : filtered) {
            auto* card = Gtk::make_managed<PackageCard>(
                package,
                [this](const PackageRecord& pkg) { showPackageDetails(pkg); },
                [this](const PackageRecord& pkg) { installPackage(pkg); });
            packages_flow_.append(*card);
        }

        showPackageDetails(filtered.front());
    }

    void showPackageDetails(const PackageRecord& package) {
        selected_package_ = package;
        detail_name_.set_text(package.name);
        detail_source_.set_text(sourceDisplayName(package.source) + " • version " + package.version + " • " + packageStatusText(package));
        detail_summary_.set_text(package.summary);
        detail_desc_.set_text(package.description);
        detail_install_.set_label(package.installed ? "Update" : "Install");
    }

    void installPackage(const PackageRecord& package) {
        const std::string command = packageActionCommand(package);
        status_label_.set_text("Executing: " + command);
        const int rc = std::system(command.c_str());
        if (rc == 0) {
            const_cast<PackageRecord&>(package).installed = true;
            const_cast<PackageRecord&>(package).update_available = false;
            refreshData();
        } else {
            status_label_.set_text("Package operation failed for " + package.name + ". Verify the package source and backend availability.");
        }
    }

    void installFile(const std::filesystem::path& path) {
        const std::string command = packageOpenCommand(path);
        status_label_.set_text("Opening package file: " + path.string());
        const int rc = std::system(command.c_str());
        if (rc == 0) {
            status_label_.set_text("Package file processed successfully: " + path.string());
        } else {
            status_label_.set_text("The file could not be opened or installed. Check the package format and installed tools.");
        }
    }

    void onSelectLocalFile() {
        auto dialog = Gtk::FileChooserNative::create("Select package file", *this, Gtk::FileChooserAction::OPEN, "Open", "Cancel");
        auto filter = Gtk::FileFilter::create();
        filter->set_name("Package files");
        filter->add_pattern("*.deb");
        filter->add_pattern("*.appimage");
        filter->add_pattern("*.flatpakref");
        filter->add_pattern("*.flatpak");
        dialog->add_filter(filter);

        if (dialog->run() == Gtk::ResponseType::ACCEPT) {
            const auto file = dialog->get_file();
            if (file) {
                openPackageFile(file->get_path());
            }
        }
    }

    Gtk::Box root_{Gtk::Orientation::VERTICAL};
    Gtk::HeaderBar header_;
    Gtk::Label title_widget_{"Ultrapkg Plus"};
    Gtk::Box toolbar_{Gtk::Orientation::HORIZONTAL};
    Gtk::SearchEntry search_entry_;
    Gtk::Button refresh_button_;
    Gtk::Button local_file_button_;
    Gtk::Box main_shell_{Gtk::Orientation::HORIZONTAL};
    Gtk::Box sidebar_{Gtk::Orientation::VERTICAL};
    std::map<std::string, Gtk::Button*> sidebar_buttons_;
    Gtk::Box content_area_{Gtk::Orientation::HORIZONTAL};
    Gtk::Box packages_layout_{Gtk::Orientation::VERTICAL};
    Gtk::Label store_header_{"Discover packages"};
    Gtk::ScrolledWindow package_scroll_;
    Gtk::FlowBox packages_flow_;
    Gtk::Box detail_pane_{Gtk::Orientation::VERTICAL};
    Gtk::Label detail_name_{""};
    Gtk::Label detail_source_{""};
    Gtk::Label detail_summary_{""};
    Gtk::Label detail_desc_{""};
    Gtk::Box detail_buttons_{Gtk::Orientation::HORIZONTAL};
    Gtk::Button detail_install_;
    Gtk::Button detail_file_;
    Gtk::Label status_label_{"Ready"};
    std::vector<PackageRecord> packages_;
    std::string current_category_ = "overview";
    PackageRecord selected_package_{};
};

int main(int argc, char** argv) {
    auto app = Gtk::Application::create("org.flax.ultrapkgplus");
    auto* window = new MainWindow();

    if (argc > 1) {
        window->openPackageFile(std::filesystem::path(argv[1]));
    }

    window->present();
    return app->run();
}
