#include <iostream>
#include "AudioAmplifierEcu.h"
#include "AudioToolClient.h"

void printMenu() {
    std::cout << "\n=== Car Audio CAN Simulator ===\n";
    std::cout << "1. Set master volume\n";
    std::cout << "2. Set balance (front/rear)\n";
    std::cout << "3. Set EQ (bass/mid/treble)\n";
    std::cout << "4. Mute / Unmute\n";
    std::cout << "5. Increase temperature (simulate overheat)\n";
    std::cout << "6. Read DTCs\n";
    std::cout << "7. Clear DTCs\n";
    std::cout << "0. Exit\n";
    std::cout << "Select: ";
}

int main() {
    AudioAmplifierEcu ecu;
    AudioToolClient tool(ecu);

    while (true) {
        printMenu();
        int choice = -1;
        if (!(std::cin >> choice)) break;

        if (choice == 0) {
            break;
        }

        switch (choice) {
            case 1: {
                int v;
                std::cout << "Enter volume (0-100): ";
                std::cin >> v;
                tool.setMasterVolume(v);
                break;
            }
            case 2: {
                int f, r;
                std::cout << "Enter front balance (-50..50): ";
                std::cin >> f;
                std::cout << "Enter rear balance (-50..50): ";
                std::cin >> r;
                tool.setBalance(f, r);
                break;
            }
            case 3: {
                int b, m, t;
                std::cout << "Enter bass (0-10): ";
                std::cin >> b;
                std::cout << "Enter mid (0-10): ";
                std::cin >> m;
                std::cout << "Enter treble (0-10): ";
                std::cin >> t;
                tool.setEq(b, m, t);
                break;
            }
            case 4: {
                int x;
                std::cout << "Mute? (1=yes, 0=no): ";
                std::cin >> x;
                tool.setMute(x != 0);
                break;
            }
            case 5: {
                int d;
                std::cout << "Increase temperature by (degC): ";
                std::cin >> d;
                tool.increaseTemperature(d);
                break;
            }
            case 6: {
                tool.readDtcs();
                break;
            }
            case 7: {
                tool.clearDtcs();
                break;
            }
            default:
                std::cout << "Invalid choice\n";
                break;
        }
    }

    std::cout << "Exiting simulator.\n";
    return 0;
}
