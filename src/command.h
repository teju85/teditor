#pragma once

#include <string>
#include <unordered_map>
#include <memory>


namespace teditor {

/**
 * @brief Enum to distinguish between first and subsequent undo/redo calls
 */
enum CommandCallType {
    /** Command being called for the first time */
    CMD_FRESH = 0,
    /** Command's undo part is being called */
    CMD_UNDO,
    /** Commads' redo part is being called */
    CMD_REDO
};


class Command;

typedef Command* (*CmdCreator)();
typedef std::shared_ptr<Command> CmdPtr;
typedef std::unordered_map<std::string,CmdCreator> CmdCreatorMap;

/**
 * @brief Main class to extend functionality of the editor.
 * Use the CMD_* macros defined in this header itself to define more commands
 */
class Command {
public:
    Command(): canIundo(false) {}
    virtual ~Command() {}
    /**
     * @brief function that tells whether this command supports undo
     * @note the default value is always set in the constructor based on the
     * command's "default" behavior. However, the command is free to modify
     * this behavior by updating the 'canIundo' flag. That way, a command that
     * is naturally supports undo can be made to not support based on runtime
     * findings! See: BackspaceChar as an example of how this is used.
     */
    bool canUndo() const { return canIundo; }
    virtual void exec(CommandCallType type) = 0;

    static void registerCmd(const std::string& cmd, CmdCreator fptr);
    static CmdPtr createCmd(const std::string& cmd);

protected:
    bool canIundo;
};


/** Accessor function to the command map */
CmdCreatorMap& cmds();


class RegisterCmd {
public:
    RegisterCmd(const std::string& cmd, CmdCreator fptr) {
        Command::registerCmd(cmd, fptr);
    }
};


#define CMD_NO_UNDO(Name, CmdStr)                       \
    class Name: public Command {                        \
    public:                                             \
        void exec(CommandCallType type);                \
        static Command* create() { return new Name; }   \
    };                                                  \
    RegisterCmd cmd ## Name(CmdStr, Name::create);      \
    void Name::exec(CommandCallType type)


#define CMD_UNDO1(Name, CmdStr, var1)                   \
    class Name: public Command {                        \
    public:                                             \
        Name(): Command() { canIundo = true;}           \
        void exec(CommandCallType type);                \
        static Command* create() { return new Name; }   \
    private:                                            \
        var1;                                           \
    };                                                  \
    RegisterCmd cmd ## Name(CmdStr, Name::create);      \
    void Name::exec(CommandCallType type)


#define CMD_UNDO2(Name, CmdStr, var1, var2)             \
    class Name: public Command {                        \
    public:                                             \
        Name(): Command() { canIundo = true;}           \
        void exec(CommandCallType type);                \
        static Command* create() { return new Name; }   \
    private:                                            \
        var1;                                           \
        var2;                                           \
    };                                                  \
    RegisterCmd cmd ## Name(CmdStr, Name::create);      \
    void Name::exec(CommandCallType type)


#define CMD_UNDO3(Name, CmdStr, var1, var2, var3)       \
    class Name: public Command {                        \
    public:                                             \
        Name(): Command() { canIundo = true;}           \
        void exec(CommandCallType type);                \
        static Command* create() { return new Name; }   \
    private:                                            \
        var1;                                           \
        var2;                                           \
        var3;                                           \
    };                                                  \
    RegisterCmd cmd ## Name(CmdStr, Name::create);      \
    void Name::exec(CommandCallType type)


}; // end namespace teditor
