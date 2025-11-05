// Caesar_cipher - FINAL VERSION
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>

std::string content;
Fl_Text_Buffer *in_buf = new Fl_Text_Buffer();
Fl_Text_Buffer *out_buf = new Fl_Text_Buffer();
Fl_Text_Display *in_disp, *out_disp;
Fl_Hor_Value_Slider *slider;
Fl_Box *alpha_orig, *alpha_shift, *shift_label;
Fl_Button *mode_btn;
bool encrypt_mode = true;
int shift = 13;

std::string caesar(const std::string& s, int k, bool enc) {
    std::string r;
    int d = enc ? k : (26 - k);
    for (char c : s) {
        if (std::isalpha(c)) {
            char b = std::isupper(c) ? 'A' : 'a';
            c = (c - b + d) % 26 + b;
        }
        r += c;
    }
    return r;
}

void update() {
    out_buf->text(caesar(content, shift, encrypt_mode).c_str());

    std::string a = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z";
    std::string b;
    for (char c : a) if (c >= 'A' && c <= 'Z') {
        int i = (c - 'A' + (encrypt_mode ? shift : 26 - shift)) % 26;
        b += 'A' + i; b += ' ';
    }
    b.pop_back();
    alpha_orig->copy_label(a.c_str());
    alpha_shift->copy_label(b.c_str());
    shift_label->copy_label(std::to_string(shift).c_str());
}

void load(const std::string& p) {
    std::ifstream f(p);
    if (!f) return (void)fl_alert("Cannot open file!");
    content.assign((std::istreambuf_iterator<char>(f)), {});
    in_buf->text(content.c_str());
    update();
}

void save(const std::string& p) {
    std::ofstream f(p);
    if (!f) return (void)fl_alert("Cannot save!");
    f << out_buf->text();
}

void open_cb(Fl_Widget*, void*) {
    Fl_Native_File_Chooser fc;
    fc.title("Open Text File");
    fc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fc.filter("Text Files\t*.txt\n");
    if (fc.show() == 0) load(fc.filename());
}

void save_cb(Fl_Widget*, void*) {
    if (out_buf->length() == 0) return (void)fl_alert("Nothing to save!");
    Fl_Native_File_Chooser fc;
    fc.title("Save As");
    fc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    fc.filter("Text Files\t*.txt\n");
    fc.preset_file(encrypt_mode ? "encrypted.txt" : "decrypted.txt");
    if (fc.show() == 0) {
        std::string p = fc.filename();
        if (std::ifstream(p)) {
            if (fl_choice("Overwrite?", "No", "Yes", 0) != 1) return;
        }
        save(p);
    }
}

void slider_cb(Fl_Widget*, void*) {
    shift = (int)slider->value();
    update();
}

void mode_cb(Fl_Widget*, void*) {
    encrypt_mode = !encrypt_mode;
    mode_btn->label(encrypt_mode ? "ENCRYPT" : "DECRYPT");
    mode_btn->color(encrypt_mode ? 0x00aa0000 : 0xaa000000);
    update();
}

void drop_cb(Fl_Widget*, void*) {
    if (Fl::event_text() && strncmp(Fl::event_text(), "file://", 7) == 0) {
        std::string p = Fl::event_text() + 7;
        size_t e = p.find_first_of("\r\n");
        if (e != std::string::npos) p.resize(e);
        load(p);
    }
}

int main() {
    // Main window - resizable
    Fl_Window win(840, 720, "Caesar Cipher Text");
    win.resizable(&win);
    win.color(0x1e1e1e00);

    // FIXED: Use exit(0) instead of hide() so [X] button closes the app
    win.callback([](Fl_Widget*, void*) { exit(0); });

    // Title
    Fl_Box title(0, 15, 840, 50, "Caesar Cipher Text");
    title.labelsize(36);
    title.labelcolor(FL_WHITE);
    title.align(FL_ALIGN_CENTER);

    // Controls
    mode_btn = new Fl_Button(30, 75, 110, 38, "ENCRYPT");
    mode_btn->color(0x00aa0000);
    mode_btn->labelcolor(FL_WHITE);
    mode_btn->callback(mode_cb);

    Fl_Button open_btn(160, 75, 110, 38, "Open");
    open_btn.color(0x3366ff00);
    open_btn.labelcolor(FL_WHITE);
    open_btn.callback(open_cb);

    Fl_Button save_btn(290, 75, 110, 38, "Save");
    save_btn.color(0xff660000);
    save_btn.labelcolor(FL_WHITE);
    save_btn.callback(save_cb);

    // Input
    in_disp = new Fl_Text_Display(20, 135, 800, 200);
    in_disp->buffer(in_buf);
    in_disp->color(0x11111100);
    in_disp->textcolor(0xcccccc00);
    in_disp->textfont(FL_COURIER);

    // Output
    out_disp = new Fl_Text_Display(20, 355, 800, 200);
    out_disp->buffer(out_buf);
    out_disp->color(0x11111100);
    out_disp->textcolor(0xaaffaa00);
    out_disp->textfont(FL_COURIER);

    // Slider
    new Fl_Box(20, 570, 80, 30, "Shift:");
    slider = new Fl_Hor_Value_Slider(100, 575, 580, 30);
    slider->type(FL_HOR_NICE_SLIDER);
    slider->range(1, 25);
    slider->step(1);
    slider->value(13);
    slider->color(0x33333300);
    slider->selection_color(0x00aa0000);
    slider->callback(slider_cb);

    shift_label = new Fl_Box(700, 570, 100, 40, "13");
    shift_label->labelcolor(FL_WHITE);
    shift_label->labelfont(FL_BOLD);
    shift_label->align(FL_ALIGN_CENTER);

    // Alphabet Preview
    alpha_orig = new Fl_Box(20, 620, 800, 30, "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
    alpha_orig->labelcolor(0x8888ff00);
    alpha_orig->labelfont(FL_COURIER);

    alpha_shift = new Fl_Box(20, 650, 800, 30, "N O P Q R S T U V W X Y Z A B C D E F G H I J K L M");
    alpha_shift->labelcolor(0xff888800);
    alpha_shift->labelfont(FL_COURIER);

    // Footer
    new Fl_Box(0, 690, 840, 25, "Drag & drop .txt • Encrypt • Shift • Decrypt\n Made by Group C");
    ((Fl_Box*)win.child(win.children()-1))->labelcolor(0x66666600);
    ((Fl_Box*)win.child(win.children()-1))->align(FL_ALIGN_CENTER);

    win.end();
    win.show();

    // Enable drag & drop
    Fl::add_handler([](int event) -> int {
        if (event == FL_DND_RELEASE || event == FL_PASTE) {
            drop_cb(nullptr, nullptr);
            return 1;
        }
        return 0;
    });

    // Center all fl_message / fl_alert
    Fl::visual(FL_RGB);
    fl_message_font(FL_HELVETICA, 14);
    fl_message_icon()->align(FL_ALIGN_TOP);

    update();
    return Fl::run();
}