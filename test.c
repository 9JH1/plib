#include "plib.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  pl_arg *test = PL_A("--test");
  pl_arg *help =
      PL_A(.flag = "--help", .desc = "Show help message", .cat = "General",
           .type = "none", .short_flag = "-h", .takes_value = 0);
  pl_arg *version = PL_A(.flag = "--version", .desc = "Display version info",
                         .cat = "General", .type = "none", .short_flag = "-v",
                         .takes_value = 0);
  pl_arg *verbose = PL_A(.flag = "--verbose", .desc = "Enable verbose output",
                         .cat = "Output", .type = "none", .short_flag = "-V",
                         .takes_value = 0);
  pl_arg *quiet =
      PL_A(.flag = "--quiet", .desc = "Suppress all output", .cat = "Output",
           .type = "none", .short_flag = "-q", .takes_value = 0);
  pl_arg *output =
      PL_A(.flag = "--output", .desc = "Specify output file", .cat = "File",
           .type = "string", .short_flag = "-o", .takes_value = 1);
  pl_arg *input =
      PL_A(.flag = "--input", .desc = "Specify input file", .cat = "File",
           .type = "string", .short_flag = "-i", .takes_value = 1);
  pl_arg *config = PL_A(.flag = "--config", .desc = "Set configuration file",
                        .cat = "Config", .type = "path", .short_flag = "-c",
                        .takes_value = 1);
  pl_arg *log =
      PL_A(.flag = "--log", .desc = "Set log file path", .cat = "Logging",
           .type = "path", .short_flag = "-l", .takes_value = 1);
  pl_arg *debug =
      PL_A(.flag = "--debug", .desc = "Enable debug mode", .cat = "Debug",
           .type = "none", .short_flag = "-d", .takes_value = 0);
  pl_arg *force =
      PL_A(.flag = "--force", .desc = "Force operation", .cat = "General",
           .type = "none", .short_flag = "-f", .takes_value = 0);
  pl_arg *dry_run =
      PL_A(.flag = "--dry-run", .desc = "Simulate execution", .cat = "General",
           .type = "none", .short_flag = "-n", .takes_value = 0);
  pl_arg *silent =
      PL_A(.flag = "--silent", .desc = "No output", .cat = "Output",
           .type = "none", .short_flag = "-s", .takes_value = 0);
  pl_arg *file = PL_A(.flag = "--file", .desc = "Target file", .cat = "File",
                      .type = "path", .short_flag = "-F", .takes_value = 1);
  pl_arg *path = PL_A(.flag = "--path", .desc = "Set file path", .cat = "File",
                      .type = "path", .short_flag = "-p", .takes_value = 1);
  pl_arg *directory =
      PL_A(.flag = "--directory", .desc = "Set directory", .cat = "File",
           .type = "path", .short_flag = "-D", .takes_value = 1);
  pl_arg *recursive =
      PL_A(.flag = "--recursive", .desc = "Enable recursive mode",
           .cat = "Operation", .type = "none", .short_flag = "-r",
           .takes_value = 0);
  pl_arg *overwrite =
      PL_A(.flag = "--overwrite", .desc = "Overwrite existing files",
           .cat = "File", .type = "none", .short_flag = "-O", .takes_value = 0);
  pl_arg *append =
      PL_A(.flag = "--append", .desc = "Append to file", .cat = "File",
           .type = "none", .short_flag = "-a", .takes_value = 0);
  pl_arg *format =
      PL_A(.flag = "--format", .desc = "Set output format", .cat = "Output",
           .type = "string", .short_flag = "-m", .takes_value = 1);
  pl_arg *json =
      PL_A(.flag = "--json", .desc = "Use JSON format", .cat = "Output",
           .type = "none", .short_flag = "-j", .takes_value = 0);
  pl_arg *xml = PL_A(.flag = "--xml", .desc = "Use XML format", .cat = "Output",
                     .type = "none", .short_flag = "-x", .takes_value = 0);
  pl_arg *yaml =
      PL_A(.flag = "--yaml", .desc = "Use YAML format", .cat = "Output",
           .type = "none", .short_flag = "-y", .takes_value = 0);
  pl_arg *csv = PL_A(.flag = "--csv", .desc = "Use CSV format", .cat = "Output",
                     .type = "none", .short_flag = "-C", .takes_value = 0);
  pl_arg *port =
      PL_A(.flag = "--port", .desc = "Set port number", .cat = "Network",
           .type = "int", .short_flag = "-P", .takes_value = 1);
  pl_arg *host =
      PL_A(.flag = "--host", .desc = "Set host address", .cat = "Network",
           .type = "string", .short_flag = "-H", .takes_value = 1);
  pl_arg *user = PL_A(.flag = "--user", .desc = "Set username", .cat = "Auth",
                      .type = "string", .short_flag = "-u", .takes_value = 1);
  pl_arg *password =
      PL_A(.flag = "--password", .desc = "Set password", .cat = "Auth",
           .type = "string", .short_flag = "-w", .takes_value = 1);
  pl_arg *timeout =
      PL_A(.flag = "--timeout", .desc = "Set timeout value", .cat = "Network",
           .type = "int", .short_flag = "-t", .takes_value = 1);
  pl_arg *retry =
      PL_A(.flag = "--retry", .desc = "Set retry count", .cat = "Network",
           .type = "int", .short_flag = "-R", .takes_value = 1);
  pl_arg *count =
      PL_A(.flag = "--count", .desc = "Set iteration count", .cat = "Operation",
           .type = "int", .short_flag = "-n", .takes_value = 1);
  pl_arg *size =
      PL_A(.flag = "--size", .desc = "Set buffer size", .cat = "Operation",
           .type = "int", .short_flag = "-z", .takes_value = 1);
  pl_arg *limit =
      PL_A(.flag = "--limit", .desc = "Set maximum limit", .cat = "Operation",
           .type = "int", .short_flag = "-L", .takes_value = 1);
  pl_arg *offset =
      PL_A(.flag = "--offset", .desc = "Set offset value", .cat = "Operation",
           .type = "int", .short_flag = "-o", .takes_value = 1);
  pl_arg *filter =
      PL_A(.flag = "--filter", .desc = "Apply filter", .cat = "Data",
           .type = "string", .short_flag = "-f", .takes_value = 1);
  pl_arg *sort = PL_A(.flag = "--sort", .desc = "Sort results", .cat = "Data",
                      .type = "string", .short_flag = "-S", .takes_value = 1);
  pl_arg *ascending =
      PL_A(.flag = "--ascending", .desc = "Sort in ascending order",
           .cat = "Data", .type = "none", .short_flag = "-A", .takes_value = 0);
  pl_arg *descending =
      PL_A(.flag = "--descending", .desc = "Sort in descending order",
           .cat = "Data", .type = "none", .short_flag = "-E", .takes_value = 0);
  pl_arg *ignore =
      PL_A(.flag = "--ignore", .desc = "Ignore errors", .cat = "Operation",
           .type = "none", .short_flag = "-I", .takes_value = 0);
  pl_arg *include =
      PL_A(.flag = "--include", .desc = "Include patterns", .cat = "Data",
           .type = "string", .short_flag = "-i", .takes_value = 1);
  pl_arg *exclude =
      PL_A(.flag = "--exclude", .desc = "Exclude patterns", .cat = "Data",
           .type = "string", .short_flag = "-e", .takes_value = 1);
  pl_arg *pattern =
      PL_A(.flag = "--pattern", .desc = "Set pattern", .cat = "Data",
           .type = "string", .short_flag = "-p", .takes_value = 1);
  pl_arg *regex =
      PL_A(.flag = "--regex", .desc = "Use regex pattern", .cat = "Data",
           .type = "string", .short_flag = "-r", .takes_value = 1);
  pl_arg *case_sensitive =
      PL_A(.flag = "--case-sensitive", .desc = "Case sensitive search",
           .cat = "Data", .type = "none", .short_flag = "-C", .takes_value = 0);
  pl_arg *no_case =
      PL_A(.flag = "--no-case", .desc = "Case insensitive search",
           .cat = "Data", .type = "none", .short_flag = "-N", .takes_value = 0);
  pl_arg *threads = PL_A(.flag = "--threads", .desc = "Set thread count",
                         .cat = "Performance", .type = "int",
                         .short_flag = "-t", .takes_value = 1);
  pl_arg *jobs =
      PL_A(.flag = "--jobs", .desc = "Set job count", .cat = "Performance",
           .type = "int", .short_flag = "-j", .takes_value = 1);
  pl_arg *parallel =
      PL_A(.flag = "--parallel", .desc = "Enable parallel execution",
           .cat = "Performance", .type = "none", .short_flag = "-P",
           .takes_value = 0);
  pl_arg *sequential =
      PL_A(.flag = "--sequential", .desc = "Force sequential execution",
           .cat = "Performance", .type = "none", .short_flag = "-Q",
           .takes_value = 0);
  pl_arg *backup =
      PL_A(.flag = "--backup", .desc = "Create backup", .cat = "File",
           .type = "none", .short_flag = "-b", .takes_value = 0);
  pl_arg *restore =
      PL_A(.flag = "--restore", .desc = "Restore from backup", .cat = "File",
           .type = "none", .short_flag = "-R", .takes_value = 0);
  pl_arg *archive =
      PL_A(.flag = "--archive", .desc = "Create archive", .cat = "File",
           .type = "none", .short_flag = "-A", .takes_value = 0);
  pl_arg *compress =
      PL_A(.flag = "--compress", .desc = "Compress output", .cat = "File",
           .type = "none", .short_flag = "-z", .takes_value = 0);
  pl_arg *decompress =
      PL_A(.flag = "--decompress", .desc = "Decompress input", .cat = "File",
           .type = "none", .short_flag = "-d", .takes_value = 0);
  pl_arg *encrypt =
      PL_A(.flag = "--encrypt", .desc = "Encrypt data", .cat = "Security",
           .type = "none", .short_flag = "-E", .takes_value = 0);
  pl_arg *decrypt =
      PL_A(.flag = "--decrypt", .desc = "Decrypt data", .cat = "Security",
           .type = "none", .short_flag = "-D", .takes_value = 0);
  pl_arg *key =
      PL_A(.flag = "--key", .desc = "Set encryption key", .cat = "Security",
           .type = "string", .short_flag = "-k", .takes_value = 1);
  pl_arg *cert =
      PL_A(.flag = "--cert", .desc = "Set certificate", .cat = "Security",
           .type = "path", .short_flag = "-c", .takes_value = 1);
  pl_arg *ssl = PL_A(.flag = "--ssl", .desc = "Enable SSL", .cat = "Network",
                     .type = "none", .short_flag = "-S", .takes_value = 0);
  pl_arg *tls = PL_A(.flag = "--tls", .desc = "Enable TLS", .cat = "Network",
                     .type = "none", .short_flag = "-T", .takes_value = 0);
  pl_arg *proxy =
      PL_A(.flag = "--proxy", .desc = "Set proxy server", .cat = "Network",
           .type = "string", .short_flag = "-x", .takes_value = 1);
  pl_arg *no_proxy =
      PL_A(.flag = "--no-proxy", .desc = "Disable proxy", .cat = "Network",
           .type = "none", .short_flag = "-X", .takes_value = 0);
  pl_arg *url = PL_A(.flag = "--url", .desc = "Set URL", .cat = "Network",
                     .type = "string", .short_flag = "-u", .takes_value = 1);
  pl_arg *endpoint =
      PL_A(.flag = "--endpoint", .desc = "Set API endpoint", .cat = "Network",
           .type = "string", .short_flag = "-e", .takes_value = 1);
  pl_arg *api_key =
      PL_A(.flag = "--api-key", .desc = "Set API key", .cat = "Auth",
           .type = "string", .short_flag = "-K", .takes_value = 1);
  pl_arg *token =
      PL_A(.flag = "--token", .desc = "Set auth token", .cat = "Auth",
           .type = "string", .short_flag = "-t", .takes_value = 1);
  pl_arg *auth =
      PL_A(.flag = "--auth", .desc = "Enable authentication", .cat = "Auth",
           .type = "none", .short_flag = "-a", .takes_value = 0);
  pl_arg *no_auth =
      PL_A(.flag = "--no-auth", .desc = "Disable authentication", .cat = "Auth",
           .type = "none", .short_flag = "-N", .takes_value = 0);
  pl_arg *header =
      PL_A(.flag = "--header", .desc = "Set HTTP header", .cat = "Network",
           .type = "string", .short_flag = "-H", .takes_value = 1);
  pl_arg *cookie =
      PL_A(.flag = "--cookie", .desc = "Set HTTP cookie", .cat = "Network",
           .type = "string", .short_flag = "-C", .takes_value = 1);
  pl_arg *method =
      PL_A(.flag = "--method", .desc = "Set HTTP method", .cat = "Network",
           .type = "string", .short_flag = "-M", .takes_value = 1);
  pl_arg *get =
      PL_A(.flag = "--get", .desc = "Use GET method", .cat = "Network",
           .type = "none", .short_flag = "-G", .takes_value = 0);
  pl_arg *post =
      PL_A(.flag = "--post", .desc = "Use POST method", .cat = "Network",
           .type = "none", .short_flag = "-P", .takes_value = 0);
  pl_arg *put =
      PL_A(.flag = "--put", .desc = "Use PUT method", .cat = "Network",
           .type = "none", .short_flag = "-U", .takes_value = 0);
  pl_arg *delete =
      PL_A(.flag = "--delete", .desc = "Use DELETE method", .cat = "Network",
           .type = "none", .short_flag = "-D", .takes_value = 0);
  pl_arg *patch =
      PL_A(.flag = "--patch", .desc = "Use PATCH method", .cat = "Network",
           .type = "none", .short_flag = "-A", .takes_value = 0);
  pl_arg *status =
      PL_A(.flag = "--status", .desc = "Show status", .cat = "General",
           .type = "none", .short_flag = "-s", .takes_value = 0);
  pl_arg *monitor =
      PL_A(.flag = "--monitor", .desc = "Monitor process", .cat = "Operation",
           .type = "none", .short_flag = "-m", .takes_value = 0);
  pl_arg *watch =
      PL_A(.flag = "--watch", .desc = "Watch for changes", .cat = "Operation",
           .type = "none", .short_flag = "-w", .takes_value = 0);
  pl_arg *interval = PL_A(.flag = "--interval", .desc = "Set interval time",
                          .cat = "Operation", .type = "int", .short_flag = "-i",
                          .takes_value = 1);
  pl_arg *duration =
      PL_A(.flag = "--duration", .desc = "Set duration", .cat = "Operation",
           .type = "int", .short_flag = "-d", .takes_value = 1);
  pl_arg *threshold =
      PL_A(.flag = "--threshold", .desc = "Set threshold", .cat = "Operation",
           .type = "float", .short_flag = "-T", .takes_value = 1);
  pl_arg *memory =
      PL_A(.flag = "--memory", .desc = "Set memory limit", .cat = "Performance",
           .type = "int", .short_flag = "-M", .takes_value = 1);
  pl_arg *cpu =
      PL_A(.flag = "--cpu", .desc = "Set CPU limit", .cat = "Performance",
           .type = "int", .short_flag = "-C", .takes_value = 1);
  pl_arg *disk =
      PL_A(.flag = "--disk", .desc = "Set disk limit", .cat = "Performance",
           .type = "int", .short_flag = "-D", .takes_value = 1);
  pl_arg *network =
      PL_A(.flag = "--network", .desc = "Set network config", .cat = "Network",
           .type = "string", .short_flag = "-N", .takes_value = 1);
  pl_arg *interface =
      PL_A(.flag = "--interface", .desc = "Set network interface",
           .cat = "Network", .type = "string", .short_flag = "-I",
           .takes_value = 1);
  pl_arg *address =
      PL_A(.flag = "--address", .desc = "Set IP address", .cat = "Network",
           .type = "string", .short_flag = "-a", .takes_value = 1);
  pl_arg *gateway =
      PL_A(.flag = "--gateway", .desc = "Set gateway", .cat = "Network",
           .type = "string", .short_flag = "-g", .takes_value = 1);
  pl_arg *dns =
      PL_A(.flag = "--dns", .desc = "Set DNS server", .cat = "Network",
           .type = "string", .short_flag = "-n", .takes_value = 1);
  pl_arg *domain =
      PL_A(.flag = "--domain", .desc = "Set domain name", .cat = "Network",
           .type = "string", .short_flag = "-d", .takes_value = 1);
  pl_arg *email =
      PL_A(.flag = "--email", .desc = "Set email address", .cat = "Auth",
           .type = "string", .short_flag = "-e", .takes_value = 1);
  pl_arg *phone =
      PL_A(.flag = "--phone", .desc = "Set phone number", .cat = "Auth",
           .type = "string", .short_flag = "-p", .takes_value = 1);
  pl_arg *lang = PL_A(.flag = "--lang", .desc = "Set language", .cat = "Config",
                      .type = "string", .short_flag = "-l", .takes_value = 1);
  pl_arg *locale =
      PL_A(.flag = "--locale", .desc = "Set locale", .cat = "Config",
           .type = "string", .short_flag = "-L", .takes_value = 1);
  pl_arg *timezone =
      PL_A(.flag = "--timezone", .desc = "Set timezone", .cat = "Config",
           .type = "string", .short_flag = "-z", .takes_value = 1);
  pl_arg *theme =
      PL_A(.flag = "--theme", .desc = "Set UI theme", .cat = "Config",
           .type = "string", .short_flag = "-T", .takes_value = 1);
  pl_arg *color =
      PL_A(.flag = "--color", .desc = "Set color scheme", .cat = "Config",
           .type = "string", .short_flag = "-c", .takes_value = 1);
  pl_arg *font =
      PL_A(.flag = "--font", .desc = "Set font style", .cat = "Config",
           .type = "string", .short_flag = "-f", .takes_value = 1);
  pl_arg *scale =
      PL_A(.flag = "--scale", .desc = "Set UI scale", .cat = "Config",
           .type = "float", .short_flag = "-s", .takes_value = 1);
  pl_arg *width = PL_A(.flag = "--width", .desc = "Set width", .cat = "Config",
                       .type = "int", .short_flag = "-w", .takes_value = 1);
  pl_arg *height =
      PL_A(.flag = "--height", .desc = "Set height", .cat = "Config",
           .type = "int", .short_flag = "-h", .takes_value = 1);
  
	pl_r ret = PL_PROC();

  if (ret != PL_SUCCESS) {
    printf("error: %s on arg %s (%d)\n", PL_E(ret), PL_LAST_ARG, ret);

    pl_help();
  }

	pl_free();
  return 0;
}
