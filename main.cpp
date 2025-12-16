// Caesar Cipher - Alphabet shifting - Group C
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Input.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;
// --- Constants ---
const int WIN_W = 840, WIN_H = 750; // Window dimensions
const int EDITOR_W = 800, EDITOR_H = 200; // Editor dimensions
const int CONTROL_H = 38, SLIDER_W = 580; // Control heights and slider width
const int MARGIN = 20, SPACING = 15; // Layout margins and spacing
const Fl_Color BG_COLOR = 0x1e1e1e00; // Dark background
const Fl_Color BTN_COLOR = 0x3366ff00; // Button color
const Fl_Color ENCRYPT_COLOR = 0x00aa0000; // Encrypt mode color
const Fl_Color DECRYPT_COLOR = 0xa000aa00; // Decrypt mode color
const Fl_Color TEXT_COLOR = FL_WHITE; // Text color
const Fl_Color ALPHA_ORIG_COLOR = 0x8888ff00; // Original alphabet color
const Fl_Color ALPHA_SHIFT_COLOR = 0xff888800; // Shifted alphabet color

// --- Encryption Logic ---
// Project: Design an encryption and decryption algorithm using Substitution Ciphers based on the key shifting alphabet

// Namespaces help organize code and prevent naming conflicts.
namespace cipher {
    // This function encrypts or decrypts text using the Caesar cipher.
    string caesar(const string& s, int k, bool enc) {
        // This variable stores the result of encryption/decryption.
        string result;

        // This variable decides how much to shift letters: forward for encrypt, backward for decrypt.
        int direction = enc ? k : (26 - k);

        // This loop goes through each character c in the input string s.
        for (char c : s) {
            // This condition checks if the character is a letter.
            if (isalpha(c)) {
                // This variable sets the base for uppercase or lowercase letters.
                // e.g. where D - A = 3 (index of D relative to A being 0)
                char base = isupper(c) ? 'A' : 'a';
                // This expression shifts the letter and wraps around if needed.
                // e.g. so D index + the direction magnitute, modulo 26 in case of a number larger than 26 or less than 0, and the output is still a char
                c = (c - base + direction) % 26 + base;
                // in C++ chars are represented by 1 Byte data (ASCII) so they can be treated as numbers, so 'A' is 65, and 'a' is 97, but we only use alphabets here so we only care about that part of the cipher. other wise it would be faster to do the cipher.
            }
            // this adds the character to the result.
            result += c;
        }
        return result;
    }
}



// --- UI State ---
Fl_Text_Buffer *in_buf = new Fl_Text_Buffer(); // Input buffer
Fl_Text_Buffer *out_buf = new Fl_Text_Buffer(); // Output buffer
Fl_Text_Editor *in_editor, *out_editor; // Input and output editors
Fl_Hor_Value_Slider *slider; // Shift slider
Fl_Input *shift_input; // Shift input field
Fl_Box *alpha_orig, *alpha_shift; // Alphabet display
Fl_Button *mode_btn; // Mode toggle button
bool encrypt_mode = true; // Current mode: encrypt or decrypt
int shift = 13; // Current shift value

// --- UI Update Logic ---
// Updates output and alphabet display based on current input, shift, and mode
void update_ui() {
    string in_text = in_buf->text();
    out_buf->text(cipher::caesar(in_text, shift, encrypt_mode).c_str());

    string original = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z";
    string shifted;
    for (char c : original) {
        if (c >= 'A' && c <= 'Z') {
            int index = (c - 'A' + (encrypt_mode ? shift : 26 - shift)) % 26;
            shifted += 'A' + index;
            shifted += ' ';
        }
    }
    shifted.pop_back();
    alpha_orig->copy_label(original.c_str());
    alpha_shift->copy_label(shifted.c_str());
    shift_input->value(to_string(shift).c_str());
}

// --- Callbacks ---
// Called when input buffer changes
void in_buf_cb(int, int, int, int, const char*, void*) {
    update_ui();
}

// Loads text from file
void load(const string& path) {
    ifstream file(path);
    if (!file) {
        fl_alert("Cannot open file!");
        return;
    }
    in_buf->text(string((istreambuf_iterator<char>(file)), {}).c_str());
    update_ui();
}

// Saves output to file
void save(const string& path) {
    ofstream file(path);
    if (!file) {
        fl_alert("Cannot save!");
        return;
    }
    file << out_buf->text();
}

// Opens file dialog and loads selected file
void open_cb(Fl_Widget*, void*) {
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Open Text File");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    file_chooser.filter("Text Files\t*.txt\n");
    if (file_chooser.show() == 0) {
        load(file_chooser.filename());
    }
}

// Opens save dialog and saves output
void save_cb(Fl_Widget*, void*) {
    if (out_buf->length() == 0) {
        fl_alert("Nothing to save!");
        return;
    }
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Save As");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    file_chooser.filter("Text Files\t*.txt\n");
    file_chooser.preset_file(encrypt_mode ? "encrypted.txt" : "decrypted.txt");
    if (file_chooser.show() == 0) {
        string path = file_chooser.filename();
        if (ifstream(path)) {
            if (fl_choice("Overwrite?", "No", "Yes", 0) != 1) {
                return;
            }
        }
        save(path);
    }
}

// Updates shift value from slider
void slider_cb(Fl_Widget*, void*) {
    shift = (int)slider->value();
    update_ui();
}

// Updates shift value from input field
void shift_input_cb(Fl_Widget*, void*) {
    shift = stoi(shift_input->value());
    slider->value(shift);
    update_ui();
}

// Toggles between encrypt and decrypt mode
void mode_cb(Fl_Widget*, void*) {
    encrypt_mode = !encrypt_mode;
    mode_btn->label(encrypt_mode ? "ENCRYPT" : "DECRYPT");
    mode_btn->color(encrypt_mode ? ENCRYPT_COLOR : DECRYPT_COLOR);
    update_ui();
}

// Handles file drop
void drop_cb(Fl_Widget*, void*) {
    if (Fl::event_text() && strncmp(Fl::event_text(), "file://", 7) == 0) {
        string path = Fl::event_text() + 7;
        size_t end = path.find_first_of("\r\n");
        if (end != string::npos) {
            path.resize(end);
        }
        load(path);
    }
}

// --- Main ---
int main() {
    Fl_Window window(WIN_W, WIN_H, "Caesar Cipher Text");
    window.resizable(&window);
    window.color(BG_COLOR);
    window.callback([](Fl_Widget*, void*) { exit(0); });

    // Title
    Fl_Box title(MARGIN, SPACING, WIN_W - 2*MARGIN, 50, "Caesar Cipher Text");
    title.labelsize(36);
    title.labelcolor(TEXT_COLOR);
    title.align(FL_ALIGN_CENTER);

    // Controls
    mode_btn = new Fl_Button(MARGIN, 75, 110, CONTROL_H, "ENCRYPT");
    mode_btn->color(ENCRYPT_COLOR);
    mode_btn->labelcolor(TEXT_COLOR);
    mode_btn->callback(mode_cb);

    Fl_Button open_btn(140, 75, 110, CONTROL_H, "Open");
    open_btn.color(BTN_COLOR);
    open_btn.labelcolor(TEXT_COLOR);
    open_btn.callback(open_cb);

    Fl_Button save_btn(270, 75, 110, CONTROL_H, "Save");
    save_btn.color(BTN_COLOR);
    save_btn.labelcolor(TEXT_COLOR);
    save_btn.callback(save_cb);

    // Shift Input
    Fl_Box shift_label(400, 75, 50, CONTROL_H, "Shift:");
    shift_input = new Fl_Input(450, 75, 50, CONTROL_H);
    shift_input->value("13");
    shift_input->callback(shift_input_cb);
    shift_label.labelcolor(TEXT_COLOR); // Make "Shift" label white
    shift_input->color(0x33333300);
    shift_input->textcolor(TEXT_COLOR);

    // Input (editable)
    in_editor = new Fl_Text_Editor(MARGIN, 135, EDITOR_W, EDITOR_H);
    in_editor->buffer(in_buf);
    in_buf->add_modify_callback(in_buf_cb, nullptr);
    in_editor->color(0x33333300);
    in_editor->textcolor(0xcccccc00);
    in_editor->textfont(FL_COURIER);
    in_editor->show_cursor();
    in_editor->cursor_color(0x99999900);
    in_editor->cursor_style(5);
    in_editor->take_focus();

    // Output (read-only, but with cursor)
    out_editor = new Fl_Text_Editor(MARGIN, 355, EDITOR_W, EDITOR_H);
    out_editor->buffer(out_buf);
    out_editor->color(0x33333300);
    out_editor->textcolor(0xaaffaa00);
    out_editor->textfont(FL_COURIER);
    out_editor->cursor_color(TEXT_COLOR);
    out_editor->cursor_style(FL_CURSOR_INSERT);

    // Slider
    Fl_Box* shift_slider_label = new Fl_Box(MARGIN, 570, 80, 30, "Shift:");
    shift_slider_label->labelcolor(TEXT_COLOR); // Make "Shift:" label white
    slider = new Fl_Hor_Value_Slider(100, 575, SLIDER_W, 30);
    slider->type(FL_HOR_NICE_SLIDER);
    slider->range(1, 25);
    slider->step(1);
    slider->value(13);
    slider->selection_color(ENCRYPT_COLOR);
    slider->callback(slider_cb);
    slider->color(0x33333300);
    slider->textcolor(TEXT_COLOR);

    // Alphabet Preview
    alpha_orig = new Fl_Box(MARGIN, 620, EDITOR_W, 30, "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
    alpha_orig->labelcolor(ALPHA_ORIG_COLOR);
    alpha_orig->labelfont(FL_COURIER);

    alpha_shift = new Fl_Box(MARGIN, 650, EDITOR_W, 30, "N O P Q R S T U V W X Y Z A B C D E F G H I J K L M");
    alpha_shift->labelcolor(ALPHA_SHIFT_COLOR);
    alpha_shift->labelfont(FL_COURIER);

    // Footer
    new Fl_Box(0, 690, WIN_W, 25, "Drag & drop .txt • Encrypt • Shift • Decrypt\n Made by Group C\n Mustafa Donmez, Amanda Farghli, Edison Florian, Dominick Gordon");
    ((Fl_Box*)window.child(window.children()-1))->labelcolor(0x66666600);
    ((Fl_Box*)window.child(window.children()-1))->align(FL_ALIGN_CENTER);

    window.end();
    window.show();

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

    update_ui();
    return Fl::run();
}
