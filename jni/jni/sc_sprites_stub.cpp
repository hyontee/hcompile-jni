#include <string>

// Текстуры — доп функции
std::string png2sc(const std::string& png, const std::string& orig, int idx, const std::string& out) {
    return "⏳ PNG в SC — в разработке";
}
std::string sc_extract_ktx(const std::string& sc, int idx, const std::string& out) {
    return "⏳ Извлечь KTX — в разработке";
}

// Спрайты
std::string cut_sprites(const std::string& logic, const std::string& tex, const std::string& out) {
    return "⏳ Нарезка спрайтов — в разработке";
}
std::string sc2png(const std::string& sc, int idx, const std::string& out) {
    return "⏳ SC в PNG — в разработке";
}
std::string sc_exports(const std::string& sc, const std::string& out) {
    return "⏳ Список экспортов — в разработке";
}

// Инжект
std::string inject_preview(const std::string& sc, const std::string& out) {
    return "⏳ Превью UV — в разработке";
}
std::string inject_put(const std::string& sc, const std::string& sprite, const std::string& out) {
    return "⏳ Внедрить спрайт — в разработке";
}
std::string inject_list(const std::string& sc) {
    return "⏳ Список экспортов — в разработке";
}

// Сборка
std::string anim2static(const std::string& sc, const std::string& out) {
    return "⏳ Анимация в статику — в разработке";
}
std::string build_sc(const std::string& folder, const std::string& out) {
    return "⏳ Сборка SC — в разработке";
}
std::string mc_copy(const std::string& src, const std::string& dst, const std::string& out) {
    return "⏳ MC Copy — в разработке";
}
