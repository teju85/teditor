#pragma once

#include "buffer.h"


namespace teditor {

/** Functor to filter input string against a list of choices */
typedef bool(*ChoicesFilter)(const std::string&, const std::string&);

/** Default ChoicesFilter which just performs std::string::find */
bool strFind(const std::string& line, const std::string& str);


class Choices {
public:
    Choices(ChoicesFilter cf): filter(cf), optLoc(-1) {}
    virtual ~Choices() {}
    virtual const std::string& at(int idx) const = 0;
    virtual int size() const = 0;
    virtual bool updateChoices(const std::string& str) { return false; }
    virtual std::string getFinalStr(int idx, const std::string& str) const = 0;
    bool match(const std::string& line, const std::string& str) const;
    bool match(int idx, const std::string& str) const;
    void setIdx(int idx) { optLoc = idx; }
    int getIdx() const { return optLoc; }

private:
    ChoicesFilter filter;
    int optLoc;
};


class StringChoices: public Choices {
public:
    StringChoices(const Strings& arr, ChoicesFilter cf=strFind);
    const std::string& at(int idx) const { return options[idx]; }
    std::string getFinalStr(int idx, const std::string& str) const;
    int size() const { return (int)options.size(); }

protected:
    Strings options;
};


class CmdMsgBar: public Buffer {
public:
    CmdMsgBar();
    void resize(const Pos2d<int>& start, const Pos2d<int>& dim) override;
    void insert(char c) override;
    void insert(const char* str) override;
    void drawBuffer(Editor& ed) override;
    void load(const std::string& file, int line=0) override {}
    void drawStatusBar(Editor& ed) override {}
    void save() override {}
    void clear() override;
    void setMinLoc(int loc) { minLoc = loc; }
    int getMinStartLoc() const override { return minLoc; }
    int totalLinesNeeded() const override;
    void setChoices(Choices* ch) { choices = ch; }
    void clearChoices();
    bool usingChoices() const { return choices != nullptr; }
    std::string getStr() const { return lines[0].get().substr(minLoc); }
    std::string getFinalChoice() const;
    void down();
    void up();
    void updateChoices();
    int getOptLoc() const { return optLoc; }
    void lineDown();

private:
    /** useful during prompts, so as to not cross into the message itself! */
    int minLoc;
    /** external choices vector (usually options) that need to be rendered */
    Choices* choices;
    /** currently selected option */
    int optLoc;

    int linesNeeded(const std::string& str, int wid) const;
};

} // end namespace teditor
