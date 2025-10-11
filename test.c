#include "plib.h"
#include <stdio.h>

int main(const int c, const char *v[]) {
  pl_arg *my_arg = PL_P("--my-arg", "my argument!", .takes_value = 1);
  pl_arg *help = PL_P("--help", "Display help information", .takes_value = 0,
                      .category = "General", .shorthand = "-h");
  pl_arg *version = PL_P("--version", "Show version number", .takes_value = 0,
                         .category = "General", .shorthand = "-v");
  pl_arg *input_file =
      PL_P("--input-file", "Specify input file path", .takes_value = 1,
           .category = "File", .shorthand = "-i", .type = "string");
  pl_arg *output_dir =
      PL_P("--output-dir", "Set output directory", .takes_value = 1,
           .category = "File", .shorthand = "-o", .type = "string");
  pl_arg *port =
      PL_P("--port", "Network port to use", .takes_value = 1,
           .category = "Network", .shorthand = "-p", .type = "integer");
  pl_arg *timeout =
      PL_P("--timeout", "Set connection timeout", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *log_file =
      PL_P("--log-file", "Specify log file path", .takes_value = 1,
           .category = "Debug", .type = "string");
  pl_arg *config =
      PL_P("--config", "Path to configuration file", .takes_value = 1,
           .category = "General", .shorthand = "-c", .type = "string");
  pl_arg *dry_run = PL_P("--dry-run", "Simulate execution without changes",
                         .takes_value = 0, .category = "General");
  pl_arg *host =
      PL_P("--host", "Specify server host", .takes_value = 1,
           .category = "Network", .shorthand = "-H", .type = "string");
  pl_arg *debug_level =
      PL_P("--debug-level", "Set debug verbosity level", .takes_value = 1,
           .category = "Debug", .type = "integer");
  pl_arg *overwrite = PL_P("--overwrite", "Overwrite existing files",
                           .takes_value = 0, .category = "File");
  pl_arg *protocol =
      PL_P("--protocol", "Set network protocol", .takes_value = 1,
           .category = "Network", .type = "string");
  pl_arg *quiet = PL_P("--quiet", "Suppress output", .takes_value = 0,
                       .category = "General", .shorthand = "-q");
  pl_arg *backup = PL_P("--backup", "Create backup of files", .takes_value = 0,
                        .category = "File");
  pl_arg *retry =
      PL_P("--retry", "Number of retries for network", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *trace = PL_P("--trace", "Enable trace logging", .takes_value = 0,
                       .category = "Debug");
  pl_arg *format = PL_P("--format", "Output format", .takes_value = 1,
                        .category = "General", .type = "string");
  pl_arg *archive = PL_P("--archive", "Archive output files", .takes_value = 0,
                         .category = "File");
  pl_arg *bind = PL_P("--bind", "Bind to specific address", .takes_value = 1,
                      .category = "Network", .type = "string");
  pl_arg *log_level =
      PL_P("--log-level", "Set logging level", .takes_value = 1,
           .category = "Debug", .shorthand = "-l", .type = "string");
  pl_arg *force = PL_P("--force", "Force operation", .takes_value = 0,
                       .category = "General", .shorthand = "-f");
  pl_arg *compress = PL_P("--compress", "Enable compression", .takes_value = 0,
                          .category = "File");
  pl_arg *max_connections =
      PL_P("--max-connections", "Max network connections", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *error_log = PL_P("--error-log", "Path to error log", .takes_value = 1,
                           .category = "Debug", .type = "string");
  pl_arg *recursive =
      PL_P("--recursive", "Process directories recursively", .takes_value = 0,
           .category = "File", .shorthand = "-r");
  pl_arg *ssl =
      PL_P("--ssl", "Enable SSL", .takes_value = 0, .category = "Network");
  pl_arg *profile = PL_P("--profile", "Specify user profile", .takes_value = 1,
                         .category = "General", .type = "string");
  pl_arg *append =
      PL_P("--append", "Append to files", .takes_value = 0, .category = "File");
  pl_arg *buffer_size =
      PL_P("--buffer-size", "Set buffer size", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *debug_mode = PL_P("--debug-mode", "Run in debug mode",
                            .takes_value = 0, .category = "Debug");
  pl_arg *no_color = PL_P("--no-color", "Disable colored output",
                          .takes_value = 0, .category = "General");
  pl_arg *file_type = PL_P("--file-type", "Specify file type", .takes_value = 1,
                           .category = "File", .type = "string");
  pl_arg *retry_delay =
      PL_P("--retry-delay", "Delay between retries", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *log_format = PL_P("--log-format", "Set log format", .takes_value = 1,
                            .category = "Debug", .type = "string");
  pl_arg *silent =
      PL_P("--silent", "Run silently", .takes_value = 0, .category = "General");
  pl_arg *chmod = PL_P("--chmod", "Set file permissions", .takes_value = 1,
                       .category = "File", .type = "string");
  pl_arg *proxy = PL_P("--proxy", "Set proxy server", .takes_value = 1,
                       .category = "Network", .type = "string");
  pl_arg *trace_file =
      PL_P("--trace-file", "Path to trace file", .takes_value = 1,
           .category = "Debug", .type = "string");
  pl_arg *no_cache = PL_P("--no-cache", "Disable caching", .takes_value = 0,
                          .category = "General");
  pl_arg *max_size = PL_P("--max-size", "Max file size", .takes_value = 1,
                          .category = "File", .type = "integer");
  pl_arg *keep_alive = PL_P("--keep-alive", "Enable keep-alive",
                            .takes_value = 0, .category = "Network");
  pl_arg *stack_trace = PL_P("--stack-trace", "Enable stack tracing",
                             .takes_value = 0, .category = "Debug");
  pl_arg *theme = PL_P("--theme", "Set output theme", .takes_value = 1,
                       .category = "General", .type = "string");
  pl_arg *exclude = PL_P("--exclude", "Exclude files or dirs", .takes_value = 1,
                         .category = "File", .type = "string");
  pl_arg *bandwidth = PL_P("--bandwidth", "Limit bandwidth", .takes_value = 1,
                           .category = "Network", .type = "integer");
  pl_arg *log_dir = PL_P("--log-dir", "Directory for logs", .takes_value = 1,
                         .category = "Debug", .type = "string");
  pl_arg *interactive = PL_P("--interactive", "Enable interactive mode",
                             .takes_value = 0, .category = "General");
  pl_arg *preserve = PL_P("--preserve", "Preserve file attributes",
                          .takes_value = 0, .category = "File");
  pl_arg *cert = PL_P("--cert", "Path to SSL certificate", .takes_value = 1,
                      .category = "Network", .type = "string");
  pl_arg *debug_output =
      PL_P("--debug-output", "Debug output destination", .takes_value = 1,
           .category = "Debug", .type = "string");
  pl_arg *no_prompt = PL_P("--no-prompt", "Disable prompts", .takes_value = 0,
                           .category = "General");
  pl_arg *rename = PL_P("--rename", "Rename files", .takes_value = 1,
                        .category = "File", .type = "string");
  pl_arg *timeout_read =
      PL_P("--timeout-read", "Read timeout", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *log_verbose =
      PL_P("--log-verbose", "Verbose log settings", .takes_value = 1,
           .category = "Debug", .type = "string");
  pl_arg *summary = PL_P("--summary", "Show summary", .takes_value = 0,
                         .category = "General");
  pl_arg *move = PL_P("--move", "Move files instead of copy", .takes_value = 0,
                      .category = "File");
  pl_arg *dns = PL_P("--dns", "Specify DNS server", .takes_value = 1,
                     .category = "Network", .type = "string");
  pl_arg *trace_level =
      PL_P("--trace-level", "Set trace verbosity", .takes_value = 1,
           .category = "Debug", .type = "integer");
  pl_arg *json_output = PL_P("--json-output", "Output in JSON format",
                             .takes_value = 0, .category = "General");
  pl_arg *filter = PL_P("--filter", "Filter files", .takes_value = 1,
                        .category = "File", .type = "string");
  pl_arg *max_retries =
      PL_P("--max-retries", "Max retry attempts", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *log_append = PL_P("--log-append", "Append to log file",
                            .takes_value = 0, .category = "Debug");
  pl_arg *no_overwrite = PL_P("--no-overwrite", "Prevent overwriting",
                              .takes_value = 0, .category = "File");
  pl_arg *auth = PL_P("--auth", "Authentication credentials", .takes_value = 1,
                      .category = "Network", .type = "string");
  pl_arg *debug_all = PL_P("--debug-all", "Enable all debug options",
                           .takes_value = 0, .category = "Debug");
  pl_arg *color = PL_P("--color", "Enable colored output", .takes_value = 0,
                       .category = "General");
  pl_arg *pattern = PL_P("--pattern", "File pattern to match", .takes_value = 1,
                         .category = "File", .type = "string");
  pl_arg *socket = PL_P("--socket", "Socket path", .takes_value = 1,
                        .category = "Network", .type = "string");
  pl_arg *log_time = PL_P("--log-time", "Include timestamps in logs",
                          .takes_value = 0, .category = "Debug");
  pl_arg *force_color = PL_P("--force-color", "Force colored output",
                             .takes_value = 0, .category = "General");
  pl_arg *chmod_recursive =
      PL_P("--chmod-recursive", "Recursive permissions", .takes_value = 1,
           .category = "File", .type = "string");
  pl_arg *timeout_write =
      PL_P("--timeout-write", "Write timeout", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *log_size = PL_P("--log-size", "Max log file size", .takes_value = 1,
                          .category = "Debug", .type = "integer");
  pl_arg *no_summary = PL_P("--no-summary", "Disable summary output",
                            .takes_value = 0, .category = "General");
  pl_arg *copy =
      PL_P("--copy", "Copy files", .takes_value = 0, .category = "File");
  pl_arg *gateway = PL_P("--gateway", "Network gateway", .takes_value = 1,
                         .category = "Network", .type = "string");
  pl_arg *debug_filter =
      PL_P("--debug-filter", "Filter debug output", .takes_value = 1,
           .category = "Debug", .type = "string");
  pl_arg *output_file =
      PL_P("--output-file", "Specify output file", .takes_value = 1,
           .category = "File", .shorthand = "-O", .type = "string");
  pl_arg *retry_interval =
      PL_P("--retry-interval", "Interval between retries", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *log_rotate = PL_P("--log-rotate", "Enable log rotation",
                            .takes_value = 0, .category = "Debug");
  pl_arg *no_interactive = PL_P("--no-interactive", "Disable interactive mode",
                                .takes_value = 0, .category = "General");
  pl_arg *include = PL_P("--include", "Include files or dirs", .takes_value = 1,
                         .category = "File", .type = "string");
  pl_arg *secure = PL_P("--secure", "Enable secure mode", .takes_value = 0,
                        .category = "Network");
  pl_arg *debug_stack = PL_P("--debug-stack", "Enable stack debugging",
                             .takes_value = 0, .category = "Debug");
  pl_arg *pretty = PL_P("--pretty", "Pretty print output", .takes_value = 0,
                        .category = "General");
  pl_arg *temp_dir =
      PL_P("--temp-dir", "Set temporary directory", .takes_value = 1,
           .category = "File", .type = "string");
  pl_arg *max_bandwidth =
      PL_P("--max-bandwidth", "Max bandwidth limit", .takes_value = 1,
           .category = "Network", .type = "integer");
  pl_arg *log_backup = PL_P("--log-backup", "Backup log files",
                            .takes_value = 0, .category = "Debug");
  pl_arg *no_progress = PL_P("--no-progress", "Disable progress bar",
                             .takes_value = 0, .category = "General");
  pl_arg *file_size =
      PL_P("--file-size", "Set file size limit", .takes_value = 1,
           .category = "File", .type = "integer");
  pl_arg *interface = PL_P("--interface", "Network interface", .takes_value = 1,
                           .category = "Network", .type = "string");
  pl_arg *log_clear = PL_P("--log-clear", "Clear log files", .takes_value = 0,
                           .category = "Debug");
  pl_arg *strict = PL_P("--strict", "Enable strict mode", .takes_value = 0,
                        .category = "General");
  pl_arg *symlink = PL_P("--symlink", "Create symlinks", .takes_value = 0,
                         .category = "File");
  pl_arg *cert_key =
      PL_P("--cert-key", "Path to certificate key", .takes_value = 1,
           .category = "Network", .type = "string");
  pl_arg *debug_summary = PL_P("--debug-summary", "Show debug summary",
                               .takes_value = 0, .category = "Debug");
  pl_r return_code;

  if ((return_code = pl_proc(c, v)) == PL_SUCCESS) {
    if (PL_R(my_arg))
      printf("Argument run with value %s\n",
             pl_arg_value(my_arg, 0)); // get argument value
  } else {

    // error occured
    printf("Error occured %s from %s\n", PL_E(return_code), PL_LAST_ARG);
    pl_help();
  }

  return 0;
}
