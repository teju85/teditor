@page tutorials Tutorials

@section tutorials_intro Introduction
This is the entry page for all usage-related information about `teditor`. It
also comes in-built documentation about its usage. Inside the editor, type
`M-x help-command <command-name>` to know more about a given command. Sooner,
we'll be deprecating this command, in favor of more visual and descriptive
doxygen-based documentation of all these commands.

@section tutorial_all_commands All Commands
A Command is an operation that changes any of the following:
1. contents of a Buffer
2. filesystem
3. command-status bar
4. visual content of the editor session
5. or launches another Command

These commands can either be entered via keystrokes attached to them or via the
`M-x <command-name> Enter` command itself.

@subsection tutorial_all_commands_core Core Commands
Given below are groups of core commands that are currently supported:
1. @ref buffer_ops
2. @ref cmbar_ops
3. @ref cursor_ops
4. @ref editor_ops

@subsection tutorial_all_commands_extensions Extensions
Given below are the groups of extensions that are currently supported:
1. @ref calc_ops
2. @ref dir_ops
3. @ref grep_ops
4. @ref ledger_ops
