#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <cstdio>
#include <cstring>

#define SCREEN_WIDTH      960
#define SCREEN_HEIGHT     560

#define MAX_TEXT          65536
#define MAX_PATH_LEN      512
#define MAX_STATUS        256

#define TOP_BAR_HEIGHT    42
#define STATUS_HEIGHT     24
#define EDITOR_PADDING    10
#define FONT_SIZE         20
#define FONT_SPACING      1
#define LINE_GAP          4

enum DialogMode
{
    DIALOG_NONE = 0,
    DIALOG_OPEN,
    DIALOG_SAVE_AS
};

static int MinInt(int a, int b)
{
    return (a < b) ? a : b;
}

static int MaxInt(int a, int b)
{
    return (a > b) ? a : b;
}

static int ClampInt(int value, int minValue, int maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}


static bool IsKeyPressedOrHeld(int key)
{
    // Custom key repeat for editor controls.
    // First action happens immediately, then repeats while the key is held.
    const double repeatDelay = 0.32;     // seconds before repeating starts
    const double repeatRate = 0.045;     // seconds between repeats
    static double nextRepeatTime[512] = { 0 };

    if ((key < 0) || (key >= 512)) return IsKeyPressed(key);

    double now = GetTime();

    if (IsKeyPressed(key))
    {
        nextRepeatTime[key] = now + repeatDelay;
        return true;
    }

    if (IsKeyDown(key))
    {
        if (now >= nextRepeatTime[key])
        {
            nextRepeatTime[key] = now + repeatRate;
            return true;
        }
    }
    else
    {
        nextRepeatTime[key] = 0.0;
    }

    return false;
}

static int GetLineStart(const char *text, int pos)
{
    while ((pos > 0) && (text[pos - 1] != '\n')) pos--;
    return pos;
}

static int GetLineEnd(const char *text, int len, int pos)
{
    while ((pos < len) && (text[pos] != '\n')) pos++;
    return pos;
}

static void GetCursorRowCol(const char *text, int cursor, int *row, int *col)
{
    *row = 0;
    *col = 0;

    for (int i = 0; i < cursor; i++)
    {
        if (text[i] == '\n')
        {
            (*row)++;
            *col = 0;
        }
        else
        {
            (*col)++;
        }
    }
}

static int CountLines(const char *text, int len)
{
    int lines = 1;

    for (int i = 0; i < len; i++)
    {
        if (text[i] == '\n') lines++;
    }

    return lines;
}

static void MoveCursorUp(const char *text, int len, int *cursor)
{
    (void)len;

    int currentStart = GetLineStart(text, *cursor);
    int wantedCol = *cursor - currentStart;

    if (currentStart == 0) return;

    int prevEnd = currentStart - 1;              // char before current line is '\n'
    int prevStart = GetLineStart(text, prevEnd);
    int prevLen = prevEnd - prevStart;

    *cursor = prevStart + MinInt(wantedCol, prevLen);
}

static void MoveCursorDown(const char *text, int len, int *cursor)
{
    int currentStart = GetLineStart(text, *cursor);
    int currentEnd = GetLineEnd(text, len, *cursor);
    int wantedCol = *cursor - currentStart;

    if (currentEnd >= len) return;

    int nextStart = currentEnd + 1;
    int nextEnd = GetLineEnd(text, len, nextStart);
    int nextLen = nextEnd - nextStart;

    *cursor = nextStart + MinInt(wantedCol, nextLen);
}

static void InsertTextAtCursor(char *text, int *len, int *cursor, const char *insertText)
{
    int insertLen = static_cast<int>(std::strlen(insertText));

    if (insertLen <= 0) return;
    if ((*len + insertLen) >= MAX_TEXT) return;

    std::memmove(text + *cursor + insertLen, text + *cursor, (*len - *cursor) + 1);
    std::memcpy(text + *cursor, insertText, insertLen);

    *cursor += insertLen;
    *len += insertLen;
}

static void InsertCharAtCursor(char *text, int *len, int *cursor, char c)
{
    char temp[2] = { c, '\0' };
    InsertTextAtCursor(text, len, cursor, temp);
}

static void BackspaceAtCursor(char *text, int *len, int *cursor)
{
    if (*cursor <= 0) return;

    std::memmove(text + (*cursor - 1), text + *cursor, (*len - *cursor) + 1);

    (*cursor)--;
    (*len)--;
}

static void DeleteAtCursor(char *text, int *len, int cursor)
{
    if (cursor >= *len) return;

    std::memmove(text + cursor, text + cursor + 1, (*len - cursor));
    (*len)--;
}

static void ClearEditor(char *text, int *len, int *cursor)
{
    text[0] = '\0';
    *len = 0;
    *cursor = 0;
}

static bool SaveEditorFile(const char *fileName, char *text, char *status)
{
    if ((fileName == nullptr) || (fileName[0] == '\0'))
    {
        std::snprintf(status, MAX_STATUS, "No file name selected.");
        return false;
    }

    if (SaveFileText(fileName, text))
    {
        std::snprintf(status, MAX_STATUS, "Saved: %s", fileName);
        return true;
    }

    std::snprintf(status, MAX_STATUS, "Save failed: %s", fileName);
    return false;
}

static bool OpenEditorFile(const char *fileName, char *text, int *len, int *cursor, char *status)
{
    if ((fileName == nullptr) || (fileName[0] == '\0'))
    {
        std::snprintf(status, MAX_STATUS, "No file name selected.");
        return false;
    }

    char *loadedText = LoadFileText(fileName);

    if (loadedText == nullptr)
    {
        std::snprintf(status, MAX_STATUS, "Open failed: %s", fileName);
        return false;
    }

    std::strncpy(text, loadedText, MAX_TEXT - 1);
    text[MAX_TEXT - 1] = '\0';

    *len = static_cast<int>(std::strlen(text));
    *cursor = 0;

    UnloadFileText(loadedText);

    std::snprintf(status, MAX_STATUS, "Opened: %s", fileName);
    return true;
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mini Notepad - raylib/raygui C++");
    SetExitKey(0);
    SetTargetFPS(60);

    char text[MAX_TEXT] = { 0 };
    int textLen = 0;
    int cursor = 0;

    char currentFile[MAX_PATH_LEN] = { 0 };
    char pathInput[MAX_PATH_LEN] = { 0 };
    char status[MAX_STATUS] = "Ready. Ctrl+O Open | Ctrl+S Save | Ctrl+N New";

    int scrollLine = 0;
    DialogMode dialogMode = DIALOG_NONE;

    Font font = GetFontDefault();
    const int lineHeight = FONT_SIZE + LINE_GAP;

    while (!WindowShouldClose())
    {
        const bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

        Rectangle topBar = { 0.0f, 0.0f, static_cast<float>(GetScreenWidth()), static_cast<float>(TOP_BAR_HEIGHT) };
        Rectangle statusBar = { 0.0f, static_cast<float>(GetScreenHeight() - STATUS_HEIGHT), static_cast<float>(GetScreenWidth()), static_cast<float>(STATUS_HEIGHT) };
        Rectangle editor = {
            10.0f,
            static_cast<float>(TOP_BAR_HEIGHT + 10),
            static_cast<float>(GetScreenWidth() - 20),
            static_cast<float>(GetScreenHeight() - TOP_BAR_HEIGHT - STATUS_HEIGHT - 20)
        };

        int row = 0;
        int col = 0;
        GetCursorRowCol(text, cursor, &row, &col);

        int visibleLines = static_cast<int>((editor.height - EDITOR_PADDING*2)/lineHeight);
        if (visibleLines < 1) visibleLines = 1;

        // Keyboard shortcuts and editor typing only work when dialog is closed.
        if (dialogMode == DIALOG_NONE)
        {
            if (ctrl && IsKeyPressed(KEY_N))
            {
                ClearEditor(text, &textLen, &cursor);
                currentFile[0] = '\0';
                scrollLine = 0;
                std::snprintf(status, MAX_STATUS, "New empty document.");
            }

            if (ctrl && IsKeyPressed(KEY_O))
            {
                pathInput[0] = '\0';
                dialogMode = DIALOG_OPEN;
            }

            if (ctrl && IsKeyPressed(KEY_S))
            {
                if (currentFile[0] == '\0')
                {
                    pathInput[0] = '\0';
                    dialogMode = DIALOG_SAVE_AS;
                }
                else
                {
                    SaveEditorFile(currentFile, text, status);
                }
            }

            // Cursor movement. These keys repeat while held.
            if (IsKeyPressedOrHeld(KEY_LEFT))  cursor = MaxInt(0, cursor - 1);
            if (IsKeyPressedOrHeld(KEY_RIGHT)) cursor = MinInt(textLen, cursor + 1);
            if (IsKeyPressedOrHeld(KEY_UP))    MoveCursorUp(text, textLen, &cursor);
            if (IsKeyPressedOrHeld(KEY_DOWN))  MoveCursorDown(text, textLen, &cursor);

            if (IsKeyPressed(KEY_HOME)) cursor = GetLineStart(text, cursor);
            if (IsKeyPressed(KEY_END))  cursor = GetLineEnd(text, textLen, cursor);

            // Erase text. Backspace/Delete repeat while held.
            if (IsKeyPressedOrHeld(KEY_BACKSPACE)) BackspaceAtCursor(text, &textLen, &cursor);
            if (IsKeyPressedOrHeld(KEY_DELETE))    DeleteAtCursor(text, &textLen, cursor);

            // New line and tab. Enter repeats while held.
            if (IsKeyPressedOrHeld(KEY_ENTER) || IsKeyPressedOrHeld(KEY_KP_ENTER)) InsertCharAtCursor(text, &textLen, &cursor, '\n');
            if (IsKeyPressed(KEY_TAB)) InsertTextAtCursor(text, &textLen, &cursor, "    ");

            // Printable text input.
            int key = GetCharPressed();
            while (key > 0)
            {
                if (!ctrl && (key >= 32) && (key <= 126))
                {
                    InsertCharAtCursor(text, &textLen, &cursor, static_cast<char>(key));
                }

                key = GetCharPressed();
            }

            // Mouse wheel scroll inside editor.
            if (CheckCollisionPointRec(GetMousePosition(), editor))
            {
                int wheel = static_cast<int>(GetMouseWheelMove());
                if (wheel != 0) scrollLine -= wheel;
            }
        }

        GetCursorRowCol(text, cursor, &row, &col);

        // Keep cursor visible.
        if (row < scrollLine) scrollLine = row;
        if (row >= scrollLine + visibleLines) scrollLine = row - visibleLines + 1;

        int totalLines = CountLines(text, textLen);
        scrollLine = ClampInt(scrollLine, 0, MaxInt(0, totalLines - 1));

        BeginDrawing();
        ClearBackground(Color{ 245, 245, 245, 255 });

        // Top toolbar.
        DrawRectangleRec(topBar, Color{ 235, 235, 235, 255 });

        if (GuiButton(Rectangle{ 10.0f, 7.0f, 80.0f, 28.0f }, "New"))
        {
            ClearEditor(text, &textLen, &cursor);
            currentFile[0] = '\0';
            scrollLine = 0;
            std::snprintf(status, MAX_STATUS, "New empty document.");
        }

        if (GuiButton(Rectangle{ 100.0f, 7.0f, 80.0f, 28.0f }, "Open"))
        {
            pathInput[0] = '\0';
            dialogMode = DIALOG_OPEN;
        }

        if (GuiButton(Rectangle{ 190.0f, 7.0f, 80.0f, 28.0f }, "Save"))
        {
            if (currentFile[0] == '\0')
            {
                pathInput[0] = '\0';
                dialogMode = DIALOG_SAVE_AS;
            }
            else
            {
                SaveEditorFile(currentFile, text, status);
            }
        }

        if (GuiButton(Rectangle{ 280.0f, 7.0f, 90.0f, 28.0f }, "Save As"))
        {
            pathInput[0] = '\0';
            dialogMode = DIALOG_SAVE_AS;
        }

        GuiLabel(Rectangle{ 390.0f, 7.0f, 540.0f, 28.0f }, currentFile[0] ? currentFile : "Untitled document");

        // Editor background.
        DrawRectangleRec(editor, RAYWHITE);
        DrawRectangleLinesEx(editor, 1.0f, GRAY);

        BeginScissorMode(static_cast<int>(editor.x) + EDITOR_PADDING,
                         static_cast<int>(editor.y) + EDITOR_PADDING,
                         static_cast<int>(editor.width) - EDITOR_PADDING*2,
                         static_cast<int>(editor.height) - EDITOR_PADDING*2);

        // Draw visible text lines.
        int drawRow = 0;
        int lineStart = 0;

        for (int i = 0; i <= textLen; i++)
        {
            if ((i == textLen) || (text[i] == '\n'))
            {
                if ((drawRow >= scrollLine) && (drawRow < scrollLine + visibleLines))
                {
                    int lineLen = i - lineStart;
                    char lineBuffer[4096] = { 0 };

                    if (lineLen > 0)
                    {
                        int copyLen = MinInt(lineLen, static_cast<int>(sizeof(lineBuffer)) - 1);
                        std::memcpy(lineBuffer, text + lineStart, copyLen);
                        lineBuffer[copyLen] = '\0';
                    }

                    Vector2 pos = {
                        editor.x + EDITOR_PADDING,
                        editor.y + EDITOR_PADDING + (drawRow - scrollLine)*lineHeight
                    };

                    DrawTextEx(font, lineBuffer, pos, FONT_SIZE, FONT_SPACING, BLACK);
                }

                drawRow++;
                lineStart = i + 1;
            }
        }

        // Draw blinking cursor.
        if (dialogMode == DIALOG_NONE)
        {
            bool showCursor = (static_cast<int>(GetTime()*2.0) % 2) == 0;

            if (showCursor && (row >= scrollLine) && (row < scrollLine + visibleLines))
            {
                int cursorLineStart = GetLineStart(text, cursor);
                int prefixLen = cursor - cursorLineStart;

                char prefix[4096] = { 0 };
                if (prefixLen > 0)
                {
                    int copyLen = MinInt(prefixLen, static_cast<int>(sizeof(prefix)) - 1);
                    std::memcpy(prefix, text + cursorLineStart, copyLen);
                    prefix[copyLen] = '\0';
                }

                Vector2 prefixSize = MeasureTextEx(font, prefix, FONT_SIZE, FONT_SPACING);

                float cursorX = editor.x + EDITOR_PADDING + prefixSize.x;
                float cursorY = editor.y + EDITOR_PADDING + (row - scrollLine)*lineHeight;

                DrawRectangle(static_cast<int>(cursorX), static_cast<int>(cursorY), 2, FONT_SIZE, BLACK);
            }
        }

        EndScissorMode();

        // Status bar.
        char statusText[512] = { 0 };
        std::snprintf(statusText, sizeof(statusText),
                      "Line %d, Column %d | %s | Lines: %d | Chars: %d",
                      row + 1, col + 1, status, totalLines, textLen);

        GuiStatusBar(statusBar, statusText);

        // Open / Save dialog.
        if (dialogMode != DIALOG_NONE)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.75f));

            const char *title = (dialogMode == DIALOG_OPEN) ? "Open file" : "Save file as";
            const char *message = (dialogMode == DIALOG_OPEN) ? "Enter path/name of .txt file:" : "Enter path/name to save:";

            int result = GuiTextInputBox(Rectangle{ GetScreenWidth()/2.0f - 170.0f, GetScreenHeight()/2.0f - 70.0f, 340.0f, 140.0f },
                                         title, message, "OK;Cancel", pathInput, MAX_PATH_LEN - 1, nullptr);

            if (result == 1)       // OK
            {
                if (dialogMode == DIALOG_OPEN)
                {
                    if (OpenEditorFile(pathInput, text, &textLen, &cursor, status))
                    {
                        std::strncpy(currentFile, pathInput, MAX_PATH_LEN - 1);
                        currentFile[MAX_PATH_LEN - 1] = '\0';
                        scrollLine = 0;
                    }
                }
                else if (dialogMode == DIALOG_SAVE_AS)
                {
                    if (SaveEditorFile(pathInput, text, status))
                    {
                        std::strncpy(currentFile, pathInput, MAX_PATH_LEN - 1);
                        currentFile[MAX_PATH_LEN - 1] = '\0';
                    }
                }

                pathInput[0] = '\0';
                dialogMode = DIALOG_NONE;
            }
            else if ((result == 0) || (result == 2))     // Close / Cancel
            {
                pathInput[0] = '\0';
                dialogMode = DIALOG_NONE;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
