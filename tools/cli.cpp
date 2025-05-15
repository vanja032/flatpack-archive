// cli.cpp

#include "../include/flatpack_archive/archive.hpp"

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
)" << "\nUsage:\n";

  const std::string shared_create =
      "\tflatpack create -i <input_folder> -o <output_file>\n"
      "  \t\t[--compress | -c]\n"
      "  \t\t[--encrypt <password> | -e <password>]\n";

  const std::string shared_extract =
      "\tflatpack extract -i <archive_file> -o <output_folder>\n"
      "  \t\t[--decrypt <password> | -d <password>]\n";

  if (command == "create") {
    std::cout << shared_create << std::endl;
  } else if (command == "extract") {
    std::cout << shared_extract << std::endl;
  } else {
    std::cout << shared_create << shared_extract << "\tflatpack --help | -h\n"
              << std::endl;
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
  bool compress = false, encrypt = false, decrypt = false, help = false;

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
      {"--input",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -i/--input\n";
           std::exit(1);
         }
         input = argv[i];
       }},
      {"-o",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -o/--output\n";
           std::exit(1);
         }
         output = argv[i];
       }},
      {"--output",
       [&](int &i) {
         if (++i >= argc) {
           std::cerr << "Missing argument for -o/--output\n";
           std::exit(1);
         }
         output = argv[i];
       }},
      {"-c", [&](int &) { compress = true; }},
      {"--compress", [&](int &) { compress = true; }},
      {"-e",
       [&](int &i) {
         encrypt = true;
         if (++i >= argc) {
           std::cerr << "Missing argument for -e/--encrypt\n";
           std::exit(1);
         }
         password = argv[i];
       }},
      {"--encrypt",
       [&](int &i) {
         encrypt = true;
         if (++i >= argc) {
           std::cerr << "Missing argument for -e/--encrypt\n";
           std::exit(1);
         }
         password = argv[i];
       }},
      {"-d",
       [&](int &i) {
         decrypt = true;
         if (++i >= argc) {
           std::cerr << "Missing argument for -d/--decrypt\n";
           std::exit(1);
         }
         password = argv[i];
       }},
      {"--decrypt",
       [&](int &i) {
         decrypt = true;
         if (++i >= argc) {
           std::cerr << "Missing argument for -d/--decrypt\n";
           std::exit(1);
         }
         password = argv[i];
       }},
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
    flatpack_archive::create_archive(input, output);
    std::cout << "Flatpack archive " << output << " created." << std::endl;
  } else if (command == "extract") {
    if (input.empty() || output.empty() || help) {
      show_help("extract");
      return 0;
    }
    flatpack_archive::extract_archive(input, output);
    std::cout << "Flatpack archive " << input << " extracted." << std::endl;
  }
  return 0;
}
