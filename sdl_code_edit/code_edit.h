/*
** SDL Code Edit
**
** Copyright (C) 2018 Wang Renxin
**
** A code edit widget in plain SDL.
**
** For the latest info, see https://github.com/paladin-t/sdl_code_edit/
*/

#ifndef __CODE_EDIT_H__
#define __CODE_EDIT_H__

#include <array>
#include <assert.h>
#include <functional>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
** {========================================================
** Code edit
*/

struct CodeEdit {
public:
	enum class PaletteIndex : uint8_t {
		Default,
		Keyword,
		Number,
		String,
		CharLiteral,
		Punctuation,
		Preprocessor,
		Identifier,
		KnownIdentifier,
		PreprocIdentifier,
		Comment,
		MultiLineComment,
		Background,
		Cursor,
		Selection,
		ErrorMarker,
		Breakpoint,
		LineNumber,
		CurrentLineFill,
		CurrentLineFillInactive,
		CurrentLineEdge,
		LineEdited,
		LineEditedSaved,
		LineEditedReverted,
		Max
	};

	enum ShortcutType {
		UndoRedo = 1 << 0,
		CopyCutPaste = 1 << 2,
		All = UndoRedo | CopyCutPaste
	};

	struct Vec2 {
		float x = 0.0f, y = 0.0f;

		Vec2() {
			x = y = 0.0f;
		}
		Vec2(float _x, float _y) {
			x = _x;
			y = _y;
		}
		const float &operator[] (size_t idx) const {
			assert(idx <= 1);

			return (&x)[idx];
		}
		float &operator[] (size_t idx) {
			assert(idx <= 1);

			return (&x)[idx];
		}
	};

	struct Breakpoint {
		int line = -1;
		bool enabled = false;
		std::string condition;
	};
	
	struct Coordinates {
		int line = 0, column = 0;

		Coordinates() {
		}
		Coordinates(int ln, int col) : line(ln), column(col) {
			assert(ln >= 0);
			assert(col >= 0);
		}
		static Coordinates invalid(void) {
			static Coordinates invalid(-1, -1);
			
			return invalid;
		}

		bool operator == (const Coordinates &o) const {
			return line == o.line && column == o.column;
		}
		bool operator != (const Coordinates &o) const {
			return line != o.line || column != o.column;
		}
		bool operator < (const Coordinates &o) const {
			if (line != o.line)
				return line < o.line;

			return column < o.column;
		}
		bool operator > (const Coordinates &o) const {
			if (line != o.line)
				return line > o.line;

			return column > o.column;
		}
		bool operator <= (const Coordinates &o) const {
			if (line != o.line)
				return line < o.line;

			return column <= o.column;
		}
		bool operator >= (const Coordinates &o) const {
			if (line != o.line)
				return line > o.line;

			return column >= o.column;
		}
	};

	struct Identifier {
		Coordinates location;
		std::string declaration;
	};

	typedef std::unordered_map<std::string, Identifier> Identifiers;

	typedef std::unordered_set<std::string> Keywords;

	typedef std::map<int, std::string> ErrorMarkers;

	typedef std::unordered_set<int> Breakpoints;

	typedef std::array<unsigned, (size_t)PaletteIndex::Max> Palette;

	typedef unsigned short CodePoint;

	typedef unsigned Char; // UTF-8.

	typedef int Key;

	typedef int32_t Keycode;

	struct Glyph {
		CodePoint codepoint = 0;
		Char character = 0;
		PaletteIndex colorIndex : 7;
		bool multiLineComment : 1;

		Glyph(Char ch, PaletteIndex idx);
	};

	enum class LineState : uint8_t {
		None,
		Edited,
		EditedSaved,
		EditedReverted
	};

	struct Line : public std::vector<Glyph> {
		LineState changed = LineState::None;

		void clear(void);
		void change(void);
		void save(void);
		void revert(void);
	};

	typedef std::vector<Line> Lines;

	struct LanguageDefinition {
		typedef std::pair<std::string, PaletteIndex> TokenRegexString;

		typedef std::vector<TokenRegexString> TokenRegexStrings;

		std::string name;
		Keywords keys;
		Identifiers ids;
		Identifiers preprocIds;
		std::string commentStart, commentEnd;
		Char commentException = '\0';

		TokenRegexStrings tokenRegexPatterns;

		bool caseSensitive;

		static LanguageDefinition AngelScript(void);
		static LanguageDefinition C(void);
		static LanguageDefinition CPlusPlus(void);
		static LanguageDefinition GLSL(void);
		static LanguageDefinition HLSL(void);
		static LanguageDefinition Lua(void);
		static LanguageDefinition SQL(void);
		static LanguageDefinition BASIC8(void);
	};

	typedef std::function<bool(Key)> KeyPressed;

	typedef std::function<void(bool)> Colorized;

	typedef std::function<void(void)> Modified;

	typedef std::function<void(bool)> MouseCursorChanged;

	CodeEdit();
	virtual ~CodeEdit();

	const LanguageDefinition &getLanguageDefinition(void) const;
	LanguageDefinition &getLanguageDefinition(void);
	LanguageDefinition &setLanguageDefinition(const LanguageDefinition &langDef);

	const Palette &getPalette(void) const;
	void setPalette(const Palette &val);

	const Vec2 &getCharacterSize(void) const;
	void setCharacterSize(const Vec2 &val);

	void setErrorMarkers(const ErrorMarkers &val);
	void clearErrorMarkers(void);
	void setBreakpoints(const Breakpoints &val);
	void clearBrakpoints(void);

	void render(void* rnd);

	void setKeyPressedHandler(const KeyPressed &handler);
	void setColorizedHandler(const Colorized &handler);
	void setModifiedHandler(const Modified &handler);
	void setMouseCursorChangedHandler(const MouseCursorChanged &handler);
	void setChangesCleared(void);
	void setChangesSaved(void);

	std::vector<std::string> getTextLines(bool includeComment, bool includeString) const;
	std::string getText(const char* newLine = "\n") const;
	void setText(const std::string &txt);

	void insertText(const char* val);

	int getTotalLines(void) const;
	int getColumnsAt(int ln) const;

	Coordinates getCursorPosition(void) const;
	void setCursorPosition(const Coordinates &pos);
	void ensureCursorVisible(bool force = false);

	std::string getWordUnderCursor(void) const;
	void setSelectionStart(const Coordinates &pos);
	void setSelectionEnd(const Coordinates &pos);
	void setSelection(const Coordinates &start, const Coordinates &end, bool wordMode = false);
	void selectWordUnderCursor(void);
	void selectAll(void);
	bool hasSelection(void) const;
	void getSelection(Coordinates &start, Coordinates &end);
	std::string getSelectionText(void) const;
	int getSelectionLines(void) const;

	bool isUtf8SupportEnabled(void) const;
	void setUtf8SupportEnabled(bool val);

	bool isOverwrite(void) const;
	void setOverwrite(bool val);

	bool isReadonly(void) const;
	void setReadonly(bool val);

	bool isShortcutsEnabled(ShortcutType type) const;
	void enableShortcut(ShortcutType type);
	void disableShortcut(ShortcutType type);

	bool isTooltipEnabled(void) const;
	void setTooltipEnabled(bool en);

	void moveUp(int amount = 1, bool select = false);
	void moveDown(int amount = 1, bool select = false);
	void moveLeft(int amount = 1, bool select = false, bool wordMode = false);
	void moveRight(int amount = 1, bool select = false, bool wordMode = false);
	void moveTop(bool select = false);
	void moveBottom(bool select = false);
	void moveHome(bool select = false);
	void moveEnd(bool select = false);

	void copy(void);
	void cut(void);
	void paste(void);
	void remove(void);
	void indent(void);
	void unindent(void);

	void clearUndoRedoStack(void);
	bool canUndo(void) const;
	bool canRedo(void) const;
	void undo(int steps = 1);
	void redo(int steps = 1);

	const Vec2 &getWidgetPos(void) const;
	void setWidgetPos(const Vec2 &pos);
	const Vec2 &getWidgetSize(void) const;
	void setWidgetSize(const Vec2 &sz);
	bool isWidgetFocused(void) const;
	void setWidgetFocused(bool val);
	bool isWidgetHovered(void) const;
	void setWidgetHoverable(bool val);

	const Vec2 &getContentSize(void) const;
	float getScrollX(void) const;
	void setScrollX(float val);
	float getScrollY(void) const;
	void setScrollY(float val);

	unsigned getFrameCount(void) const;
	void setFrameCount(unsigned val);

	void addInputCharacter(CodePoint cp);
	void addInputCharactersUtf8(const char* utf8Chars);

	bool isKeyShiftDown(void) const;
	bool isKeyCtrlDown(void) const;
	bool isKeyAltDown(void) const;
	bool isKeyDown(Keycode kbkey) const;
	bool isKeyPressed(Keycode kbkey) const;
	void updateKeyStates(void);

	const Vec2 &getMousePos(void) const;
	bool isMousePressed(void) const;
	bool isMouseDoubleClicked(void) const;
	bool isMouseDragging(void) const;
	bool isMouseDown(void) const;
	void updateMouseStates(int mouseClickCount, const Vec2* scale);

	static const Palette &DarkPalette(void);
	static const Palette &LightPalette(void);
	static const Palette &RetroBluePalette(void);

protected:
	typedef std::vector<std::pair<std::regex, PaletteIndex> > RegexList;

	struct EditorState {
		Coordinates selectionStart;
		Coordinates selectionEnd;
		Coordinates cursorPosition;
	};

	enum class UndoType : uint8_t {
		Add,
		Remove,
		indent,
		unindent
	};

	struct UndoRecord {
		UndoRecord() {
		}
		~UndoRecord() {
		}

		bool similar(const UndoRecord* o) const;

		void undo(CodeEdit* editor);
		void redo(CodeEdit* editor);

		UndoType type;

		std::string overwritten;
		std::string content;
		Coordinates start;
		Coordinates end;

		EditorState before;
		EditorState after;
	};

	typedef std::vector<UndoRecord> UndoBuffer;

	typedef std::vector<uint8_t> KeyStates;

	typedef std::basic_string<CodePoint, std::char_traits<CodePoint>, std::allocator<CodePoint> > InputBuffer;

	void colorize(int fromLine = 0, int lines = -1);
	void colorizeRange(int fromLine = 0, int toLine = 0);
	void colorizeInternal(void);
	int textDistanceToLineStart(const Coordinates &from) const;
	int getPageSize(void) const;
	Coordinates getActualCursorCoordinates(void) const;
	Coordinates sanitizeCoordinates(const Coordinates &val) const;
	void advance(Coordinates &val) const;
	int getCharacterWidth(const Glyph &g) const;
	Coordinates screenPosToCoordinates(const Vec2 &pos) const;
	bool isOnWordBoundary(const Coordinates &at) const;
	void addUndo(UndoRecord &val);
	std::string getText(const Coordinates &start, const Coordinates &end, const char* newLine = "\n") const;
	int appendBuffer(std::string &buf, const Glyph &g, int idx, int &width);
	int insertTextAt(Coordinates &where, const char* val);
	void removeRange(const Coordinates &start, const Coordinates &end);
	void removeSelection(void);
	Line &insertLine(int idx);
	void removeLine(int start, int end);
	void removeLine(int idx);
	void backspace(void);
	void enterCharacter(Char ch);
	Coordinates findWordStart(const Coordinates &from) const;
	Coordinates findWordEnd(const Coordinates &from) const;
	std::string getWordAt(const Coordinates &coords) const;
	Char getCharUnderCursor(void) const;
	bool onKeyPressed(Key key);
	void onColorized(bool multilineComment) const;
	void onModified(void) const;
	void onChanged(const Coordinates &start, const Coordinates &end, int offset);

	Lines _codeLines;
	float _lineSpacing = 1.0f;
	EditorState _state;
	UndoBuffer _undoBuf;
	int _undoIndex = 0;
	int _savedIndex = 0;
	KeyPressed _keyPressedHandler;
	Colorized _colorizedHandler;
	Modified _modifiedHandler;
	MouseCursorChanged _mouseCursorChangedHandler;

	Vec2 _charAdv;
	int _tabSize = 4;
	int _textStart = 7;
	bool _utf8SupportEnabled = false;
	bool _overwrite = false;
	bool _readonly = false;
	ShortcutType _shortcutsEnabled = ShortcutType::All;
	bool _withinRender = false;
	int _scrollToCursor = 0;
	bool _wordSelectionMode = false;
	int _colorRangeMin = 0, _colorRangeMax = 0;
	int _checkMultilineComments = 0;
	bool _tooltipEnabled = true;

	Breakpoints _breakpoints;
	ErrorMarkers _errorMarkers;
	Coordinates _interactiveStart, _interactiveEnd;

	LanguageDefinition _langDef;
	Palette _palette;
	Vec2 _characterSize = Vec2(8, 8);
	RegexList _regexes;

	Vec2 _widgetPos;
	Vec2 _widgetSize;
	bool _widgetFocused = true;
	bool _widgetHoverable = true;

	Vec2 _contentSize;
	float _scrollX = 0.0f;
	float _scrollY = 0.0f;

	unsigned _frameCount = 0;

	InputBuffer _inputCharacters;

	bool _keyShift = false;
	bool _keyCtrl = false;
	bool _keyAlt = false;
	KeyStates _keyStates0;
	KeyStates _keyStates1;
	mutable std::vector<long long> _keyTimestamps;

	Vec2 _mousePos;
	bool _mousePressed = false;
	int _mouseClickCount = 0;
	bool _mouseDragged = false;
	bool _mouseDown = false;
	Vec2 _mouseDownPos;

	bool _mouseCursorInput = false;
};

/* ========================================================} */

#endif /* __CODE_EDIT_H__ */
