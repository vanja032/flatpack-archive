// cli.cpp

#include "../include/flatpack_archive/archive.hpp"
#include "../include/flatpack_archive/compression.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <string>

void print_usage(std::string command = "") {
  std::cout << R"(
  ______ _       _                    _                        _     _             _______          _ 
 |  ____| |     | |                  | |        /\            | |   (_)           |__   __|        | |
 | |__  | | __ _| |_ _ __   __ _  ___| | __    /  \   _ __ ___| |__  ___   _____     | | ___   ___ | |
 |  __| | |/ _` | __| '_ \ / _` |/ __| |/ /   / /\ \ | '__/ __| '_ \| \ \ / / _ \    | |/ _ \ / _ \| |
 | |    | | (_| | |_| |_) | (_| | (__|   <   / ____ \| | | (__| | | | |\ V /  __/    | | (_) | (_) | |
 |_|    |_|\__,_|\__| .__/ \__,_|\___|_|\_\ /_/    \_\_|  \___|_| |_|_| \_/ \___|    |_|\___/ \___/|_|
                    | |                                                                               
                    |_|                                                                               
)" << "\nUsage:\n\n";

  const std::string shared_create =
      " flatpack create -i <input_folder> -o <output_file>\n"
      "  \t[-c | --compress][default: zlib] (optional - enables compression)\n"
      "  \t[-a <compression_type> | --algo "
      "<compression_type>][zlib/zstd/lz4/brotli] (optional - enables "
      "compression algorithm)\n"
      "  \t[-e <encryption_type> | --encrypt <encryption_type>][aes256] "
      "(optional - enables encryption with encryption_type)\n"
      "  \t[-p <password> | --password <password>][default: aes256] (optional "
      "- enables encryption with password)\n"
      "  \t[-v | --verbose] (optional - enable verbose output)\n"
      "\n";

  const std::string shared_extract =
      " flatpack extract -i <archive_file> -o <output_folder>\n"
      "  \t[-p <password> | --password <password>] (optional - enables "
      "decryption with password)\n"
      "  \t[-v | --verbose] (optional - enable verbose output)\n"
      "\n";

  const std::string help = " flatpack --help | -h\n";

  if (command == "create") {
    std::cout << shared_create << std::endl;
  } else if (command == "extract") {
    std::cout << shared_extract << std::endl;
  } else {
    std::cout << shared_create << shared_extract << help << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2 || std::string(argv[1]) == "--help" ||
      std::string(argv[1]) == "-h") {
    print_usage();
    return 0;
  }

  std::string command = argv[1];
  std::string input, output, password;
  flatpack_archive::CompressionType compression_type =
      flatpack_archive::CompressionType::None;
  flatpack_archive::EncryptionType encryption_type =
      flatpack_archive::EncryptionType::None;
  bool compress = false, encrypt = false, decrypt = false, help = false,
       verbose = false;

  auto show_help = [](std::string cmd) {
    print_usage(cmd);
    std::exit(0);
  };

  std::map<std::string, std::function<void(int &)>> handlers = {
      {"-i",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -i/--input\n";
           std::exit(1);
         }
         input = argv[i];
       }},
      {"--input", [&](int &i) { handlers["-i"](i); }},
      {"-o",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -o/--output\n";
           std::exit(1);
         }
         output = argv[i];
       }},
      {"--output", [&](int &i) { handlers["-o"](i); }},
      {"-c",
       [&](int &i) {
         compress = true;
         if (compression_type == flatpack_archive::CompressionType::None)
           compression_type = flatpack_archive::CompressionType::Default;
       }},
      {"--compress", [&](int &i) { handlers["-c"](i); }},
      {"-a",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -a/--algo\n";
           std::exit(1);
         }
         compress = true;
         compression_type = flatpack_archive::parse_compression_type(argv[i]);
       }},
      {"--algo", [&](int &i) { handlers["-a"](i); }},
      {"-p",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -p/--password\n";
           std::exit(1);
         }
         encrypt = true;
         if (encryption_type == flatpack_archive::EncryptionType::None)
           encryption_type = flatpack_archive::EncryptionType::Default;
         password = argv[i];
       }},
      {"--password", [&](int &i) { handlers["-p"](i); }},
      {"-e",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -e/--encrypt\n";
           std::exit(1);
         }
         encrypt = true;
         encryption_type = flatpack_archive::parse_encryption_type(argv[i]);
       }},
      {"--encrypt", [&](int &i) { handlers["-e"](i); }},
      {"-v", [&](int &) { verbose = true; }},
      {"--verbose", [&](int &) { verbose = true; }},
      {"-h", [&](int &) { help = true; }},
      {"--help", [&](int &) { help = true; }}};

  for (int i = 2; i < argc; ++i) {
    std::string arg = argv[i];
    auto arg_it = handlers.find(arg);
    if (arg_it != handlers.end()) {
      arg_it->second(i);
    } else {
      std::cerr << "Unknown argument: " << arg << std::endl;
      print_usage();
      return 1;
    }
  }

  if (command == "create") {
    if (input.empty() || output.empty() || help) {
      show_help("create");
      return 0;
    }
    try {
      flatpack_archive::create_archive(input, output, compression_type,
                                       encryption_type, password, verbose);
      std::cout << "Flatpack archive " << output << " created." << std::endl;
    } catch (const std::exception &ex) {
      std::cerr << "Error creating archive: " << ex.what() << std::endl;
      return 1;
    }
  } else if (command == "extract") {
    if (input.empty() || output.empty() || help) {
      show_help("extract");
      return 0;
    }
    try {
      flatpack_archive::extract_archive(input, output, password, verbose);
      std::cout << "Flatpack archive " << input << " extracted." << std::endl;
    } catch (const std::exception &ex) {
      std::cerr << "Error extracting archive: " << ex.what() << std::endl;
      return 1;
    }
  }
  return 0;
}
