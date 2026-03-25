#include <ArsTimoris/Game/Map/Encounter.h>
#include <fstream>

std::vector<Encounter> LoadEncountersData(std::filesystem::path a_path) {
    std::vector<Encounter> loadedEncounters = std::vector<Encounter>();
    std::ifstream reader = std::ifstream(a_path);
    char buffer[256];
    char current = 0;
    std::string stringStorage = "";
    std::string weightStorage = "";
    std::string eventStorage = "";
    int32_t depth = 0;
    bool inString = false;
    bool inCode = false;
    uint32_t step = 0;

    bool running = true;

    while (running) {
        //std::cout << std::format("Step {}: ({:0>2}) [{:0>4}] {}", step, depth, (size_t)reader.tellg(), current) << std::endl;
        switch (step) {
            case 0:
                switch (current = reader.get()) {
                    case '{': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                ++depth;
                            }
                            weightStorage += current;
                        } else {
                            depth = 0;
                            inCode = true;
                        }
                        break;
                    }
                    case '"': {
                        if(inCode) {
                            inString = !inString;
                            weightStorage += current;
                        }
                        break;
                    }
                    case '}': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                --depth;
                                if (depth < 0) {
                                    inCode = false;
                                    depth = 0;
                                    ++step;
                                    //std::cout << weightStorage << std::endl;
                                    reader.getline(buffer, 256);
                                    //std::cout << buffer << std::endl;
                                    reader.getline(buffer, 256);
                                    //std::cout << buffer << std::endl;
                                } else {
                                    weightStorage += current;
                                }
                            } else {
                                weightStorage += current;
                            }
                        }
                        break;
                    }
                    default: {
                        if (inCode) {
                            weightStorage += current;
                        }
                        break;
                    }
                }
                break;
            case 1:
                switch (current = reader.get()) {
                    case '{': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                ++depth;
                            }
                            eventStorage += current;
                        } else {
                            inCode = true;
                        }
                        break;
                    }
                    case '"': {
                        if(inCode) {
                            inString = !inString;
                            eventStorage += current;
                        }
                        break;
                    }
                    case '}': {
                        //std::cout << std::format("Hit Bracket {}InCode\x1b[39m {}InString\x1b[39m Depth: {:0>2}", (inCode ? "\x1b[32m" : "\x1b[31m"), (inString ? "\x1b[32m" : "\x1b[31m"), depth) << std::endl;
                        if (inCode) {
                            if (!inString) {
                                --depth;
                                if (depth < 0) {
                                    depth = 0;
                                    inCode = false;
                                    //std::cout << eventStorage << std::endl;
                                    loadedEncounters.push_back(Encounter {weightStorage, eventStorage});
                                    weightStorage = "";
                                    eventStorage = "";
                                    reader.getline(buffer, 256);
                                    //std::cout << "Skipping: " << buffer << std::endl;
                                    reader.getline(buffer, 256);
                                    //std::cout << "Skipping: " << buffer << std::endl;
                                    reader.getline(buffer, 256, ';');
                                    stringStorage = buffer;
                                    //std::cout << "Checking: " << stringStorage << std::endl;
                                    if (stringStorage == "FINAL") {
                                        //std::cout << "Finished" << std::endl;
                                        running = false;
                                    } else {
                                        step = 0;
                                    }
                                    reader.getline(buffer, 256);
                                    //std::cout << buffer << std::endl;
                                } else {
                                    eventStorage += current;
                                }
                            } else {
                                eventStorage += current;
                            }
                        }
                        break;
                    }
                    default: {
                        if (inCode) {
                            eventStorage += current;
                        }
                        break;
                    }
                }
                break;
        }
    }

    reader.close();
    
    return loadedEncounters;
}