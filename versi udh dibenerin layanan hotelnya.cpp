#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <stack>
#include <array>
#include <queue>
#include <cstdlib>
#include <cstdlib>
#include <windows.h>
#include <limits>
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <time.h>
#include <ctime>

using namespace std;

stack<int> pageStack;

struct Evaluation {
    int rating;
    char comment[500];
};

struct Order1 {
    string serviceName;
    int quantity;
    string itemName;
    string itemType;
    double price;
};

struct BedType {
    string bedType;
    double pricePerNight;

    BedType(const string& bed, double price) : bedType(bed), pricePerNight(price) {}
};

struct RoomType {
    string roomType;
    vector<BedType> bedTypes;

    RoomType(const string& room) : roomType(room) {}

    void addBedType(const string& bed, double price) {
        bedTypes.push_back(BedType(bed, price));
    }
};

struct Pembayaran {
    string metodePembayaran;
    string nomorKartuKredit;
    string emailAtauNomorHP;
};

struct Reservation {
    int roomNumber; // Nomor kamar terkait dengan reservasi
    string roomType;
    string bedType;
    string checkInDate;
    string checkOutDate;
    double totalPrice; // Harga total reservasi
    Pembayaran pembayaran;  // Tambahkan informasi pembayaran

    int calculateDuration() const {
        // Konversi tanggal check-in dan check-out ke dalam format waktu (epoch)
        tm checkInTime = {}, checkOutTime = {};
        istringstream checkInStream(checkInDate);
        istringstream checkOutStream(checkOutDate);
        checkInStream >> std::get_time(&checkInTime, "%Y-%m-%d");
        checkOutStream >> std::get_time(&checkOutTime, "%Y-%m-%d");

        // Menghitung selisih waktu (epoch) antara check-out dan check-in
        time_t checkInEpoch = std::mktime(&checkInTime);
        time_t checkOutEpoch = std::mktime(&checkOutTime);
        time_t durationEpoch = checkOutEpoch - checkInEpoch;

        // Menghitung durasi dalam hari
        int durationDays = durationEpoch / (60 * 60 * 24);

        return durationDays;
    }
};

struct HotelService {
    int roomNumber; // Nomor kamar terkait dengan layanan hotel
    string serviceName;
    double price;
};

struct RoomFacility {
    int roomNumber; // Nomor kamar terkait dengan fasilitas kamar
    string facilityName;
    string description;
    double price; // Harga dalam rupiah
};

struct Room {
    int roomNumber;
    string roomType;
    string bedType;
    double pricePerNight;
    vector<RoomFacility> facilities;
    Pembayaran pembayaran;  // Tambahkan informasi pembayaran
};

struct User {
    string firstName;
    string lastName;
    string phoneNumber;
    string email;
    string username;
    string password;
    vector<Reservation> reservations;
    vector<HotelService> hotelServices;
    vector<RoomFacility> roomFacilities;
    vector<Evaluation> evaluations;
    vector<Room> availableRooms;
};

vector<User> users;
vector<Evaluation> evaluations;
stack<Evaluation> deletedEvaluations;

bool isUsernameTaken(const string& username) {
    for (const auto& user : users) {
        if (user.username == username) {
            return true;
        }
    }
    return false;
}

bool isValidPassword(const string& password) {
    // Memeriksa panjang password dan keberadaan huruf dan angka
    bool hasDigit = false;
    bool hasLetter = false;

    if (password.length() >= 8) {
        for (char ch : password) {
            if (isdigit(ch)) {
                hasDigit = true;
            } else if (isalpha(ch)) {
                hasLetter = true;
            }
        }

        return hasDigit && hasLetter;
    }

    return false;
}

bool registerUser() {
    User newUser;
    cout << "DAFTAR AKUN\n";
    cout << "--------------------------------------------------\n\n";
    cout << "Nama Depan         : ";
    cin >> newUser.firstName;
    cout << "Nama Belakang      : ";
    cin >> newUser.lastName;
    cout << "Nomor HP           : ";
    cin >> newUser.phoneNumber;
    cout << "Email              : ";
    cin >> newUser.email;

    do {
        cout << "Username           : ";
        cin >> newUser.username;

        if (isUsernameTaken(newUser.username)) {
            cout << "Username is already taken. Please choose another one.\n";
        }
    } while (isUsernameTaken(newUser.username));

    cout << "Password           : ";
    cin >> newUser.password;
    cout << "\n--------------------------------------------------\n";

    int randomRoomNumber = generateRandomRoomNumber();

    // Initialize available rooms (for demonstration purposes)
    newUser.availableRooms = displayRoomInformation();

    users.push_back(newUser);
    cout << "\nRegistration successful!\n";
    return true;
}

bool loginUser(User& loggedInUser) {
    string username, password;
    cout << "LOGIN AKUN\n";
    cout << "--------------------------------------------------\n";
    cout << "\nUsername           : ";
    cin >> username;
    cout << "Password           : ";
    cin >> password;
    cout << "\n--------------------------------------------------\n";

    for (auto& user : users) {
        if (user.username == username && user.password == password) {
            cout << "\nLogin successful!\n";
            cout << "Loading";
            Sleep(500);
            cout << ".";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            cout << ".";
            Sleep(1000);
            loggedInUser = user;
            system("cls");
            return true;
        }
    }

    cout << "\nInvalid username or password. Please try again.\n";
    return false;
}

void displayReservationHistory(const User& user) {
    if (!user.reservations.empty()) {
        cout << "Riwayat Pemesanan Anda:\n";
        cout << "--------------------------------------------------\n";

        for (const auto& reservation : user.reservations) {
            cout << "Nomor Kamar            : " << reservation.roomNumber << "\n";
            cout << "Tipe Kamar             : " << reservation.roomType << "\n";
            cout << "Tipe Tempat Tidur      : " << reservation.bedType << "\n";
            cout << "Tanggal Check-in       : " << reservation.checkInDate << "\n";
            cout << "Tanggal Check-out      : " << reservation.checkOutDate << "\n";
            cout << "Lama menginap          : " << reservation.calculateDuration() << " malam\n";

            // Display Hotel Services for this reservation
            cout << "Layanan Hotel          :\n";
            bool hasHotelServices = false;
            for (const auto& service : user.hotelServices) {
                if (service.roomNumber == reservation.roomNumber) {
                    cout << " - " << service.serviceName << " - Harga: " << fixed << setprecision(2) << service.price << " rupiah\n";
                    hasHotelServices = true;
                }
            }

            if (!hasHotelServices) {
                cout << " Tidak ada data.\n";
            }

            // Display Room Facilities for this reservation
            cout << "Fasilitas Kamar        :\n";
            bool hasRoomFacilities = false;
            for (const auto& facility : user.roomFacilities) {
                if (facility.roomNumber == reservation.roomNumber) {
                    cout << " - " << facility.facilityName << " - Harga: " << fixed << setprecision(2) << facility.price << " rupiah\n";
                    hasRoomFacilities = true;
                }
            }

            if (!hasRoomFacilities) {
                cout << " Tidak ada data.\n";
            }

            cout << "Total Price            : Rp " << fixed << setprecision(2) << reservation.totalPrice << "\n";
            cout << "--------------------------------------------------\n";
        }
    } else {
        cout << "Anda belum memiliki riwayat pemesanan.\n";
    }

    cout << "\nKembali ke halaman sebelumnya [y]?";
    char choice;
    cin >> choice;
    switch(choice) {
        case 'y':
        case 'Y':
            system("cls");
            break;
        default:
            system("cls");
    }
}

#ifdef _WIN32
void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
#else
void setConsoleColor(int color) {
    // Fungsi placeholder untuk sistem selain Windows
}
#endif

void cetakPolaBintang(int jumlahBintang) {
    // Kode untuk mengganti warna mungkin tidak berfungsi di terminal selain Windows
    // Gunakan fungsi ini hanya jika Anda yakin terminal mendukung ANSI escape codes
    // Jika tidak, Anda dapat menghapus bagian ini dan hanya mencetak bintang
#ifdef _WIN32
    setConsoleColor(14); // Warna kuning
    for (int i = 0; i < jumlahBintang; ++i) {
        cout << '*';
    }
    setConsoleColor(7); // Kembalikan warna ke default
#else
    for (int i = 0; i < jumlahBintang; ++i) {
        cout << '*';
    }
#endif
    cout << endl;
}

void beriPenilaian() {
    Evaluation newEvaluation;

    do {
        cout << "Beri penilaian (1-5 bintang): ";

        int rating;
        cin >> rating;

        if (rating < 1 || rating > 5) {
            cout << "Penilaian harus dalam rentang 1-5. Silakan coba lagi." << endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        newEvaluation.rating = rating;
        break;

    } while(true);

    cetakPolaBintang(newEvaluation.rating);

    cout << "Komentar: ";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(newEvaluation.comment, sizeof(newEvaluation.comment));

    evaluations.push_back(newEvaluation);

    cout << "Terima kasih!" << endl;
}

void lihatPenilaian() {
    cout << "Penilaian orang lain:" << endl;
    for (size_t i = 0; i < evaluations.size(); ++i) {
        cout << i + 1 << ". ";
        cetakPolaBintang(evaluations[i].rating);
        cout << " - ";
        if (strlen(evaluations[i].comment) > 0) {
            cout << evaluations[i].comment;
        } else {
            cout << "(Tidak ada komentar)";
        }
        cout << endl;
    }
}

void hapusPenilaian() {
    if (evaluations.empty()) {
        cout << "Belum ada penilaian yang bisa dihapus." << endl;
        return;
    }

    cout << "Penilaian yang bisa dihapus:" << endl;
    for (size_t i = 0; i < evaluations.size(); ++i) {
        cout << i + 1 << ". ";
        cetakPolaBintang(evaluations[i].rating);
        cout << " - ";
        if (strlen(evaluations[i].comment) > 0) {
            cout << evaluations[i].comment;
        } else {
            cout << "(Tidak ada komentar)";
        }
        cout << endl;
    }

    try {
        cout << "Pilih nomor penilaian yang ingin dihapus: ";
        int indexToDelete;
        cin >> indexToDelete;

        if (indexToDelete >= 1 && indexToDelete <= static_cast<int>(evaluations.size())) {
            --indexToDelete;
            deletedEvaluations.push(evaluations[indexToDelete]);
            evaluations.erase(evaluations.begin() + indexToDelete);
            cout << "Penilaian berhasil dihapus." << endl;
        } else {
            cout << "Nomor penilaian tidak valid." << endl;
        }
    } catch (...) {
        cout << "Terjadi kesalahan dalam menghapus penilaian." << endl;
    }
}

void kembali() {
    cout << "Kembali ke menu utama." << endl;
}

void evaluateHotel(User& user) {
    while (true) {
        cout << "\nMenu:" << endl;
        cout << "1. Beri Penilaian" << endl;
        cout << "2. Lihat Penilaian" << endl;
        cout << "3. Hapus Penilaian" << endl;
        cout << "4. Keluar" << endl;

        int pilihan;
        cout << "Pilih menu (1-4): ";
        cin >> pilihan;

        switch (pilihan) {
            case 1:
                beriPenilaian();
                break;
            case 2:
                lihatPenilaian();
                break;
            case 3:
                hapusPenilaian();
                break;
            case 4:
                #ifdef _WIN32
                system("cls");
                #else
                // Fungsi placeholder untuk sistem selain Windows
                #endif
                cout << "Anda telah keluar. Terima kasih telah menginap di hotel kami.";
                cout << endl;
                return;
            default:
                cout << "Pilihan tidak valid. Silakan pilih 1-4." << endl;
        }
    }
    system("cls");
}

void displayFoodMenu() {
    system("cls");
    cout << "====== Menu Makanan dan Minuman ======" << endl;
    cout << "1. Nasi Goreng             - Rp 50.000" << endl;
    cout << "2. Ayam goreng             - Rp 35.000" << endl;
    cout << "3. Burger                  - Rp 45.000" << endl;
    cout << "4. Pizza                   - Rp 70.000" << endl;
    cout << "5. Sate Ayam               - Rp 40.000" << endl;
    cout << "6. Soto Ayam               - Rp 45.000" << endl;
    cout << "7. Teh                     - Rp 20.000" << endl;
    cout << "8. Orange Juice            - Rp 20.000" << endl;
    cout << "9. Es Campur               - Rp 25.000" << endl;
    cout << "10. Air Mineral            - Rp 15.000" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-10): ";
}

void displayBreakfastMenu() {
    system("cls");
    cout << "=========== Menu Sarapan ===========" << endl;
    cout << "1. Nasi Goreng Spesial   - Rp 50.000" << endl;
    cout << "2. Omelette              - Rp 35.000" << endl;
    cout << "3. Bubur Ayam            - Rp 40.000" << endl;
    cout << "4. Egg Sandwich          - Rp 35.000" << endl;
    cout << "5. Pancake               - Rp 35.000" << endl;
    cout << "6. Cereal                - Rp 30.000" << endl;
    cout << "7. Kopi                  - Rp 18.000" << endl;
    cout << "8. Teh                   - Rp 16.000" << endl;
    cout << "9. Susu                  - Rp 15.000" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-9): ";
}

void displayMinibarMenu() {
    system("cls");
    cout << "======= Menu Makanan Ringan =======" << endl;
    cout << "1. Air Mineral          - Rp 15.000" << endl;
    cout << "2. Soda                 - Rp 20.000" << endl;
    cout << "3. Snack                - Rp 15.000" << endl;
    cout << "4. Coklat               - Rp 12.000" << endl;
    cout << "5. Keripik              - Rp 15.000" << endl;
    cout << "6. Permen               - Rp 10.000" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-6): ";
}

void displayLaundryMenu() {
    system("cls");
    cout << "=============== Laundry Service ===============" << endl;
    cout << "1. Reguler Laundry               - Rp 25.000/kg" << endl;
    cout << "2. Dry Cleaning                  - Rp 20.000/kg" << endl;
    cout << "3. Ironing Sevice                - Rp 15.000/kg" << endl;
    cout << "4. Express Service               - Rp 35.000/kg" << endl;
    cout << "5. Fragrance and Softener        - Rp 10.000/kg" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-6): ";
}

void displayAdditionalOrderMenu() {
    system("cls");
    cout << "========= Additional Order =========" << endl;
    cout << "1. Bantal Tambahan      - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "2. Guling Tambahan      - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "3. Alat Mandi Tambahan  - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-3): ";
}

void displayCleaningServiceMenu() {
    system("cls");
    cout << "======================= Cleaning Service =======================" << endl;
    cout << "1. General Cleaning        - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "2. Deep Cleaning           - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "3. Window Cleaning         - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "4. Carpet Cleaning         - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "5. Bed Linen Change        - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "6. Bathroom Sanitization   - GRATIS (Silakan kontak resepsionis)" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-4): ";
}

void displayMenu() {
    system("cls");
    cout << "============ Layanan Kamar ============" << endl;
    cout << "1. Menu makanan dan minuman" << endl;
    cout << "2. Menu Sarapan" << endl;
    cout << "3. Menu Minibar dan Snack" << endl;
    cout << "4. Laundry Service" << endl;
    cout << "5. Additional Order" << endl;
    cout << "6. Cleaning Service" << endl;
    cout << "0. Selesai dan Proses Pesanan" << endl;
    cout << "Pilih (0-9): ";
}

void layananKamar(vector<Order1>& orders) {
    int choice;

    do {
        displayMenu();
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            Order1 order;
            order.serviceName = "Makanan dan Minuman";

            int foodChoice;
            do {
                displayFoodMenu();
                cin >> foodChoice;

                if (foodChoice == 0) {
                    cin.clear();
                    cin.ignore();
                    break;
                } else if (foodChoice >= 1 && foodChoice <= 10) {
                    Order1 foodOrder;
                    foodOrder.serviceName = "Makanan dan Minuman";
                    cout << "Masukkan jumlah pesanan: ";
                    cin >> foodOrder.quantity;

                    switch (foodChoice) {
                        case 1:
                            foodOrder.itemName = "Nasi Goreng";
                            foodOrder.price = 50000.0;
                            break;
                        case 2:
                            foodOrder.itemName = "Ayam Goreng";
                            foodOrder.price = 35000.0;
                            break;
                        case 3:
                            foodOrder.itemName = "Burger";
                            foodOrder.price = 45000.0;
                            break;
                        case 4:
                            foodOrder.itemName = "Pizza";
                            foodOrder.price = 70000.0;
                            break;
                        case 5:
                            foodOrder.itemName = "Sate Ayam";
                            foodOrder.price = 40000.0;
                            break;
                        case 6:
                            foodOrder.itemName = "Soto ayam";
                            foodOrder.price = 45000.0;
                            break;
                        case 7:
                            foodOrder.itemName = "Teh";
                            foodOrder.price = 20000.0;
                            break;
                        case 8:
                            foodOrder.itemName = "Orang Juice";
                            foodOrder.price = 20000.0;
                            break;
                        case 9:
                            foodOrder.itemName = "Es Campur";
                            foodOrder.price = 25000.0;
                            break;
                        case 10:
                            foodOrder.itemName = "Air Mineral";
                            foodOrder.price = 15000.0;
                            break;
                    }

                    orders.push_back(foodOrder);
                    cout << "Pesanan berhasil ditambahkan!";
                } else if (foodChoice != 0) {
                    cout << "Pilihan tidak valid. Silakan coba lagi.";
                }
                    cin.ignore();
                    cin.get();
            } while (foodChoice != 0);

        } else if (choice == 2) {
            Order1 order;
            order.serviceName = "Menu Sarapan";

            int breakfastChoice;
            do {
                displayBreakfastMenu();
                cin >> breakfastChoice;

                if (breakfastChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

                } else if (breakfastChoice >= 1 && breakfastChoice <= 9) {
                    Order1 breakfastOrder;
                    breakfastOrder.serviceName = "Menu Sarapan";
                    cout << "Masukkan jumlah pesanan: ";
                    cin >> breakfastOrder.quantity;

                    switch (breakfastChoice) {
                        case 1:
                            breakfastOrder.itemName = "Nasi Goreng Spesial";
                            breakfastOrder.price = 50000.0;
                            break;
                        case 2:
                            breakfastOrder.itemName = "Omelette";
                            breakfastOrder.price = 35000.0;
                            break;
                        case 3:
                            breakfastOrder.itemName = "Bubur Ayam";
                            breakfastOrder.price = 40000.0;
                            break;
                        case 4:
                            breakfastOrder.itemName = "Egg Sandwich";
                            breakfastOrder.price = 35000.0;
                            break;
                        case 5:
                            breakfastOrder.itemName = "Pancake";
                            breakfastOrder.price = 35000.0;
                            break;
                        case 6:
                            breakfastOrder.itemName = "Cereal";
                            breakfastOrder.price = 30000.0;
                            break;
                        case 7:
                            breakfastOrder.itemName = "Kopi";
                            breakfastOrder.price = 18000.0;
                            break;
                        case 8:
                            breakfastOrder.itemName = "Teh";
                            breakfastOrder.price = 16000.0;
                            break;
                        case 9:
                            breakfastOrder.itemName = "Susu";
                            breakfastOrder.price = 15000.0;
                            break;
                    }

                    orders.push_back(breakfastOrder);
                    cout << "Pesanan berhasil ditambahkan!";
                } else if (breakfastChoice != 0) {
                    cout << "Pilihan tidak valid. Silakan coba lagi.";
                }
                    cin.ignore();
                    cin.get();
                    system("cls");
            } while (breakfastChoice != 0);

        } else if (choice == 3) {
            Order1 order;
            order.serviceName = "Makanan Ringan";

            int minibarChoice;
            do {
                displayMinibarMenu();
                cin >> minibarChoice;

                if (minibarChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

                } else if (minibarChoice >= 1 && minibarChoice <= 6) {
                    Order1 minibarOrder;
                    minibarOrder.serviceName = "Makanan Ringan";
                    cout << "Masukkan jumlah pesanan: ";
                    cin >> minibarOrder.quantity;

                    switch (minibarChoice) {
                        case 1:
                            minibarOrder.itemName = "Air Mineral";
                            minibarOrder.price = 15000.0;
                            break;
                        case 2:
                            minibarOrder.itemName = "Soda";
                            minibarOrder.price = 20000.0;
                            break;
                        case 3:
                            minibarOrder.itemName = "Snack";
                            minibarOrder.price = 15000.0;
                            break;
                        case 4:
                            minibarOrder.itemName = "Coklat";
                            minibarOrder.price = 12000.0;
                            break;
                        case 5:
                            minibarOrder.itemName = "Keripik";
                            minibarOrder.price = 15000.0;
                            break;
                        case 6:
                            minibarOrder.itemName = "Permen";
                            minibarOrder.price = 10000.0;
                            break;
                    }

                    orders.push_back(minibarOrder);
                    cout << "Pesanan berhasil ditambahkan!";
                } else if (minibarChoice != 0) {
                    cout << "Pilihan tidak valid. Silakan coba lagi.";
                }
                    cin.ignore();
                    cin.get();
                    system("cls");
            } while (minibarChoice != 0);

        } else if (choice == 4) {
            Order1 order;
            order.serviceName = "Laundry Service";

            int laundryChoice;
            do {
                displayLaundryMenu();
                cin >> laundryChoice;

                if (laundryChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

                } else if (laundryChoice >= 1 && laundryChoice <= 5) {
                    Order1 laundryOrder;
                    laundryOrder.serviceName = "Laundry Service";
                    cout << "Masukkan jumlah pesanan (kg): ";
                    cin >> laundryOrder.quantity;

                    switch (laundryChoice) {
                        case 1:
                            laundryOrder.itemName = "Reguler Laundry";
                            laundryOrder.price = 25000.0;
                            break;
                        case 2:
                            laundryOrder.itemName = "Dry Cleaning";
                            laundryOrder.price = 20000.0;
                            break;
                        case 3:
                            laundryOrder.itemName = "Ironing Service";
                            laundryOrder.price = 15000.0;
                            break;
                        case 4:
                            laundryOrder.itemName = "Express Service";
                            laundryOrder.price = 35000.0;
                            break;
                        case 5:
                            laundryOrder.itemName = "Fragrance and Softener";
                            laundryOrder.price = 10000.0;
                            break;
                    }

                    orders.push_back(laundryOrder);
                    cout << "Pesanan berhasil ditambahkan!";
                } else if (laundryChoice != 0) {
                    cout << "Pilihan tidak valid. Silakan coba lagi.";
                }
                    cin.ignore();
                    cin.get();
                    system("cls");
            } while (laundryChoice != 0);
        } else if (choice == 5) {
            Order1 order;
            order.serviceName = "Additional Order";

            int additionalOrderChoice;
            do {
                displayAdditionalOrderMenu();
                cin >> additionalOrderChoice;

                if (additionalOrderChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

                } else if (additionalOrderChoice >= 1 && additionalOrderChoice <= 3) {
                    Order1 additionalOrder;
                    additionalOrder.serviceName = "Additional Order";
                    cout << "Masukkan jumlah pesanan: ";
                    cin >> additionalOrder.quantity;

                    switch (additionalOrderChoice) {
                        case 1:
                            additionalOrder.itemName = "Bantal Tambahan";
                            additionalOrder.price = 0.0;
                            break;
                        case 2:
                            additionalOrder.itemName = "Guling Tambahan";
                            additionalOrder.price = 0.0;
                            break;
                        case 3:
                            additionalOrder.itemName = "Alat Mandi Tambahan";
                            additionalOrder.price = 0.0;
                            break;
                    }

                    orders.push_back(additionalOrder);
                    cout << "Pesanan berhasil ditambahkan!";
                } else if (additionalOrderChoice != 0) {
                    cout << "Pilihan tidak valid. Silakan coba lagi.";
                }
                    cin.ignore();
                    cin.get();
                    system("cls");
            } while (additionalOrderChoice != 0);

          } else if (choice == 6) {
            Order1 order;
            order.serviceName = "Cleaning Service";

            int cleaningServiceChoice;
            do {
                displayCleaningServiceMenu();
                cin >> cleaningServiceChoice;

                if (cleaningServiceChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

                } else if (cleaningServiceChoice >= 1 && cleaningServiceChoice <= 6) {
                    Order1 cleaningServiceOrder;
                    cleaningServiceOrder.serviceName = "Cleaning Service";
                    cleaningServiceOrder.quantity = 1;

                    switch (cleaningServiceChoice) {
                        case 1:
                            cleaningServiceOrder.itemName = "General Cleaning";
                            cleaningServiceOrder.price = 0.0;
                            break;
                        case 2:
                            cleaningServiceOrder.itemName = "Deep Cleaning";
                            cleaningServiceOrder.price = 0.0;
                            break;
                        case 3:
                            cleaningServiceOrder.itemName = "Window Cleaning";
                            cleaningServiceOrder.price = 0.0;
                            break;
                        case 4:
                            cleaningServiceOrder.itemName = "Carpet Cleaning";
                            cleaningServiceOrder.price = 0.0;
                            break;
                        case 5:
                            cleaningServiceOrder.itemName = "Bed Linen Change";
                            cleaningServiceOrder.price = 0.0;
                            break;
                        case 6:
                            cleaningServiceOrder.itemName = "Bathroom Sanitization";
                            cleaningServiceOrder.price = 0.0;
                            break;
                    }

                    orders.push_back(cleaningServiceOrder);
                    cout << "Pesanan berhasil ditambahkan!";
                } else if (cleaningServiceChoice != 0) {
                    cout << "Pilihan tidak valid. Silakan coba lagi.";
                }
                    cin.ignore();
                    cin.get();
                    system("cls");
                 } while (cleaningServiceChoice != 0);
        } else if (choice != 0) {
            cout << "Pilihan tidak valid. Silakan coba lagi.";
            cin.ignore();
            cin.get();
            system("cls");
        }
    } while (choice != 0);
    cout << "========== Ringkasan Pesanan ==========" << endl;

    double totalAmount = 0.0;
    vector<Order1> summarizedOrders;

    // Membuat ringkasan pesanan
    for (const auto &order : orders) {
        bool found = false;

        for (auto &summarizedOrder : summarizedOrders) {
            if (summarizedOrder.serviceName == order.serviceName && summarizedOrder.itemName == order.itemName) {
                summarizedOrder.quantity += order.quantity;
                found = true;
                break;
            }
        }

        if (!found) {
            summarizedOrders.push_back(order);
        }
    }

    system("cls");
    // Menampilkan ringkasan pesanan
    for (const auto &order : summarizedOrders) {
        cout << "========== Ringkasan Pesanan ==========" << endl;
        cout << "=======================================" << endl;
        cout << order.quantity << "x " << order.serviceName << endl;
        if (!order.itemName.empty()) {
            cout << " - " << order.itemName << " (Rp " << fixed << setprecision(2) << order.price << " each)" << endl;
        }
        totalAmount += order.quantity * order.price;
    }
    cout << "=======================================" << endl;

    cout << "\nTotal Tagihan: Rp " << fixed << setprecision(2) << totalAmount << endl;
    cout << "Terima kasih atas pesanan Anda!" << endl;
    cout << "Silakan melakukan pembayaran di menu Tagihan dan Pembayaran.";
    cin.get();
    system("cls");
}

void displayPoolMenu() {
    system("cls");
    cout << "Anda dapat menikmati kolam renang di hotel kami." << endl;
    cout << "Jam Operasional   : 08:00 - 20:00" << endl;
    cout << "Fasilitas         : Sauna, Jacuzzi, Handuk Gratis" << endl;
    cout << "Lantai            : 1" << endl;
    cout << "Informasi Tambahan:" << endl;
    cout << "- Kolam renang dilengkapi dengan area anak-anak." << endl;
    cout << "- Terdapat poolside bar untuk pesanan minuman dan makanan." << endl;
    cout << "- Kursi berjemur dan payung tersedia untuk tamu." << endl;
    cout << "\n=========== Menu Kolam Renang ==============" << endl;
    cout << "1. Tiket Masuk        - GRATIS" << endl;
    cout << "2. Sewa Pelampung     - Rp 20.000" << endl;
    cout << "3. Kelas Renang       - Rp 70.000/jam" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-3): ";
}

void displayRestaurantMenu() {
    system("cls");
    cout << "Nikmati makanan lezat dan minuman di restoran dan bar kami." << endl;
    cout << "Jam Operasional: 12:00 - 22:00" << endl;
    cout << "Fasilitas: Live music, Wi-Fi gratis" << endl;
    cout << "Lantai: 2" << endl;
    cout << "Informasi Tambahan:" << endl;
    cout << "- Menyajikan berbagai masakan lokal dan internasional." << endl;
    cout << "- Tempat yang nyaman untuk bersantai dengan pemandangan kota." << endl;
    cout << "- Bar menyediakan berbagai minuman, koktail, dan wine pilihan." << endl;
}

void displayFitnessMenu() {
    system("cls");
    cout << "=========== Menu Fitness ===========" << endl;
    cout << "1. Personal Training   - Rp 100.000" << endl;
    cout << "2. Yoga Class          - Rp 80.000" << endl;
    cout << "3. Exercise Group      - Rp 70.000" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-3): ";
}

void displaySpaMenu() {
    system("cls");
    cout << "=== Menu Pijat dan Spa ===" << endl;
    cout << "1. Aromatherapy Massage                                             - Rp 300.000/sesi" << endl;
    cout << "2. Hot Stone Massage                                                - Rp 350.000/sesi" << endl;
    cout << "3. Facial Treatment                                                 - Rp 250.000/sesi" << endl;
    cout << "Paket Spa:" << endl;
    cout << "4. Relaxation Package (Aromatherapy Massage + Facial Treatment)     - Rp 525.000/sesi" << endl;
    cout << "5. Wellness Package (Hot Stone Massage + Facial Treatment)          - Rp 600.000/sesi" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-5): ";
}

void displayRentalMenu() {
    system("cls");
    cout << "==== Menu Penyewaan Kendaraaan ====" << endl;
    cout << "Pilihan Penyewaan Kendaraan:" << endl;
    cout << "1. Mobil  -  Rp 300.000/hari" << endl;
    cout << "2. Mobil  -  Rp 50.000/jam" << endl;
    cout << "3. Motor  -  Rp 150.000/hari" << endl;
    cout << "4. Motor  -  Rp 25.000/jam" << endl;
    cout << "5. Sepeda -  Rp 50.000/hari" << endl;
    cout << "6. Sepeda -  Rp 10.000/jam" << endl;
    cout << "0. Selesai" << endl;
    cout << "Pilih (0-6): ";
}

void displayMenu1() {
    system("cls");
    cout << "======= Layanan Hotel ========" << endl;
    cout << "1. Kolam Renang" << endl;
    cout << "2. Restoran dan Bar" << endl;
    cout << "3. Fitness" << endl;
    cout << "4. Pijat dan Spa" << endl;
    cout << "5. Penyewaan Mobil/Motor/Sepeda" << endl;
    cout << "0. Selesai dan Proses Pesanan" << endl;
    cout << "Pilih (0-5): ";
}

void layananHotel(vector<Order1>& orders) {
    int choice;

    do {
        displayMenu1();
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            Order1 order;
            order.serviceName = "Layanan Kolam Renang";

            int poolChoice;
            do {
                displayPoolMenu();
                cin >> poolChoice;

                if (poolChoice == 0) {
                    cin.clear();
                    cin.ignore();
                    break;
                } else if (poolChoice >= 1 && poolChoice <= 3) {
                Order1 poolOrder;
                poolOrder.serviceName = "Layanan Kolam Renang";
                cout << "Masukkan jumlah pesanan: ";
                cin >> poolOrder.quantity;

                switch (poolChoice) {
                    case 1:
                        cout << "Silakan mengunjungi lantai 1." << endl;
                        break;
                    case 2:
                        poolOrder.itemName = "Sewa Pelampung";
                        poolOrder.price = 20000.0;
                        break;
                    case 3:
                        poolOrder.itemName = "Kelas Renang";
                        poolOrder.price = 70000.0;
                        break;
                }
                orders.push_back(poolOrder);
                cout << "Pesanan berhasil ditambahkan!" << endl;
            } else if (poolChoice != 0) {
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
            }
                cin.ignore();
                cin.get();
            } while (poolChoice != 0);

        } else if (choice == 2) {
            displayRestaurantMenu();
            cin.ignore();

        } else if (choice == 3) {
            Order1 order;
            order.serviceName = "Fitness";

            int fitnessChoice;
            do {
                displayFitnessMenu();
                cin >> fitnessChoice;

                if (fitnessChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

            } else if (fitnessChoice >= 1 && fitnessChoice <= 3) {
                Order1 fitnessOrder;
                fitnessOrder.serviceName = "Fitness";
                cout << "Masukkan jumlah pesanan: ";
                cin >> fitnessOrder.quantity;

                switch (fitnessChoice) {
                    case 1:
                        fitnessOrder.itemName = "Personal Trainer";
                        fitnessOrder.price = 100000.0;
                        break;
                    case 2:
                        fitnessOrder.itemName = "Yoga Class";
                        fitnessOrder.price = 80000.0;
                        break;
                    case 3:
                        fitnessOrder.itemName = "Group Excercise";
                        fitnessOrder.price = 70000.0;
                        break;
                }

                orders.push_back(fitnessOrder);
                cout << "Pesanan berhasil ditambahkan!" << endl;
            } else if (fitnessChoice != 0) {
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
            }
                cin.ignore();
                cin.get();
                system("cls");
            } while (fitnessChoice != 0);

        } else if (choice == 4) {
            Order1 order;
            order.serviceName = "Spa Service";

            int spaChoice;
            do {
                displaySpaMenu();
                cin >> spaChoice;

                if (spaChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

            } else if (spaChoice >= 1 && spaChoice <= 5) {
                Order1 spaOrder;
                spaOrder.serviceName = "Spa Service";
                cout << "Masukkan jumlah pesanan: ";
                cin >> spaOrder.quantity;

                switch (spaChoice) {
                    case 1:
                        spaOrder.itemName = "Aromatherapy Masssage";
                        spaOrder.price = 300000.0;
                        break;
                    case 2:
                        spaOrder.itemName = "Hot Stonee Massage";
                        spaOrder.price = 350000.0;
                        break;
                    case 3:
                        spaOrder.itemName = "Facial Treatment";
                        spaOrder.price = 250000.0;
                        break;
                    case 4:
                        spaOrder.itemName = "Relaxation Package";
                        spaOrder.price = 525000.0;
                        break;
                    case 5:
                        spaOrder.itemName = "Wellness Package";
                        spaOrder.price = 600000.0;
                        break;
                }

                orders.push_back(spaOrder);
                cout << "Pesanan berhasil ditambahkan!" << endl;
            } else if (spaChoice != 0) {
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
            }
                cin.ignore();
                cin.get();
                system("cls");
            } while (spaChoice != 0);

        } else if (choice == 5) {
            Order1 order;
            order.serviceName = "Rental Order";

            int rentalChoice;
            do {
                displayRentalMenu();
                cin >> rentalChoice;

                if (rentalChoice == 0) {
                cin.clear();
                cin.ignore();
                break;

            } else if (rentalChoice >= 1 && rentalChoice <= 6) {
                Order1 rentalOrder;
                rentalOrder.serviceName = "Penyewaan Mobil/Motor/Sepeda";
                cout << "Masukkan durasi penyewaan (hari/jam): ";
                cin >> rentalOrder.quantity;

                switch (rentalChoice) {
                    case 1:
                        rentalOrder.itemName = "Penyewaan Mobil Perhari";
                        rentalOrder.price = 300000.0;
                        break;
                    case 2:
                        rentalOrder.itemName = "Penyewaan Mobil Perjam";
                        rentalOrder.price = 50000.0;
                        break;
                    case 3:
                        rentalOrder.itemName = "Penyewaan Motor Pehari";
                        rentalOrder.price = 150000.0;
                        break;
                    case 4:
                        rentalOrder.itemName = "Penyewaan Motor Perjam";
                        rentalOrder.price = 25000.0;
                        break;
                    case 5:
                        rentalOrder.itemName = "Penyewaan Sepeda Perhari";
                        rentalOrder.price = 50000.0;
                        break;
                    case 6:
                        rentalOrder.itemName = "Penyewaan Sepeda Perjam";
                        rentalOrder.price = 10000.0;
                        break;
                }

                orders.push_back(rentalOrder);
                cout << "Pesanan berhasil ditambahkan!" << endl;
            } else if (rentalChoice != 0) {
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
            }
                cin.ignore();
                cin.get();
                system("cls");
        } while (rentalChoice != 0);
    } else if (choice != 0) {
        cout << "Pilihan tidak valid. Silakan coba lagi." ;
        cin.ignore();
        cin.get();
        system("cls");
    }
    } while (choice != 0);

    double totalAmount = 0.0;
    vector<Order1> summarizedOrders;

    // Membuat ringkasan pesanan
    for (const auto &order : orders) {
        bool found = false;

        for (auto &summarizedOrder : summarizedOrders) {
            if (summarizedOrder.serviceName == order.serviceName && summarizedOrder.itemName == order.itemName) {
                summarizedOrder.quantity += order.quantity;
                found = true;
                break;
            }
        }

        if (!found) {
            summarizedOrders.push_back(order);
        }
    }

    system("cls");
    // Menampilkan ringkasan pesanan
    for (const auto &order : summarizedOrders) {
        cout << "========== Ringkasan Pesanan ==========" << endl;
        cout << "=======================================" << endl;
        cout << order.quantity << "x " << order.serviceName << endl;
        if (!order.itemName.empty()) {
            cout << " - " << order.itemName << " (Rp " << fixed << setprecision(2) << order.price << " each)" << endl;
        }
        totalAmount += order.quantity * order.price;
    }
    cout << "=======================================" << endl;

    cout << "\nTotal Tagihan: Rp " << fixed << setprecision(2) << totalAmount << endl;
    cout << "Terima kasih atas pesanan Anda!" << endl;
    cout << "Silakan melakukan pembayaran di menu Tagihan dan Pembayaran.";
    cin.get();
    system("cls");
}

void showHotelServices(User& loggedInUser) {
    char serviceChoice;
    vector<Order1> ordersKamar;
    vector<Order1> ordersHotel;
    system("cls");
    do {
        cout << "Menu Layanan Hotel\n";
        cout << "--------------------------------------------------\n\n";
        cout << "1. Pesan Kamar\n";
        cout << "2. Detail Pesanan\n";
        cout << "3. Riwayat Pesanan\n";
        cout << "4. Tambah Layanan Hotel\n";
        cout << "5. Tambah Fasilitas Kamar\n";
        cout << "6. Evaluasi\n";
        cout << "7. Keluar\n\n";
        cout << "--------------------------------------------------\n";
        cout << "Pilihanmu          : ";
        cin >> serviceChoice;
        system("cls");

        switch (serviceChoice) {
            case '1':
                 Reservation newReservation;

                // Prompt the user for check-in and check-out dates
                cout << "Tanggal check-in (YYYY-MM-DD)  : ";
                cin >> newReservation.checkInDate;
                cout << "Tanggal check-out (YYYY-MM-DD) : ";
                cin >> newReservation.checkOutDate;

                // Display available rooms after getting check-in and check-out dates
                loggedInUser.availableRooms = displayRoomInformation();

                if (!loggedInUser.availableRooms.empty()) {
                    int selectedRoomNumber;
                    bool validRoom = false;

                    do {
                        // Generate a random room number
                        selectedRoomNumber = generateRandomRoomNumber();

                        // Check if the room number is valid (exists in available rooms)
                        auto it = find_if(loggedInUser.availableRooms.begin(), loggedInUser.availableRooms.end(), [selectedRoomNumber](const Room& room) {
                            return room.roomNumber == selectedRoomNumber;
                        });

                        validRoom = (it != loggedInUser.availableRooms.end());

                    } while (!validRoom);

                    auto it = find_if(loggedInUser.availableRooms.begin(), loggedInUser.availableRooms.end(), [selectedRoomNumber](const Room& room) {
                        return room.roomNumber == selectedRoomNumber;
                    });

                    if (it != loggedInUser.availableRooms.end()) {
                        // Display room type options
                        cout << endl;
                        cout << "Pilih Jenis Kamar:\n";
                        cout << "1. Deluxe\n";
                        cout << "2. Royal\n";
                        cout << "3. Suite\n";
                        cout << "Pilihan: ";
                        int jenisKamarChoice;
                        cin >> jenisKamarChoice;

                        // Validate and set the room type based on user's choice
                        switch (jenisKamarChoice) {
                            case 1:
                                newReservation.roomType = "Deluxe";
                                break;
                            case 2:
                                newReservation.roomType = "Royal";
                                break;
                            case 3:
                                newReservation.roomType = "Suite";
                                break;
                            default:
                                cout << "Pilihan tidak valid.\n";
                                return;
                        }

                        // Set the room number from the generated random number
                        newReservation.roomNumber = selectedRoomNumber;

                        // Display bed type options
                        cout << endl;
                        cout << "Pilih Jenis Tempat Tidur:\n";
                        cout << "1. Double bed\n";
                        cout << "2. Queen bed\n";
                        cout << "3. King bed\n";
                        cout << "Pilihan: ";
                        int jenisTempatTidurChoice;
                        cin >> jenisTempatTidurChoice;

                        // Validate and set the bed type based on user's choice
                        switch (jenisTempatTidurChoice) {
                            case 1:
                                newReservation.bedType = "Double bed";
                                break;
                            case 2:
                                newReservation.bedType = "Queen bed";
                                break;
                            case 3:
                                newReservation.bedType = "King bed";
                                break;
                            default:
                                cout << "Pilihan tidak valid.\n";
                                return;
                        }

                        string specialRequest;
                        cout << "\nApakah Anda memiliki permintaan khusus? (y/n) ";
                        cin >> specialRequest;

                        if (specialRequest == "y" || specialRequest == "Y") {
                            cout << "Masukkan permintaan khusus Anda: ";
                            cin.ignore();
                            getline(cin, specialRequest);

                        // Simpan permintaan khusus ke dalam objek reservasi
                        newReservation.specialRequest = specialRequest;
                        }

                        // Calculate total price based on the number of nights
                        int numNights = newReservation.calculateDuration();
                        newReservation.totalPrice = numNights * it->pricePerNight;

                        loggedInUser.reservations.push_back(newReservation);

                        // Remove the booked room from available rooms
                        loggedInUser.availableRooms.erase(it);

                        // Display reservation details
                        cout << "Berhasil Pesan Kamar!\n";
                        cout << "Loading";
                        Sleep(500);
                        cout << ".";
                        Sleep(1000);
                        cout << ".";
                        Sleep(1000);
                        cout << ".";
                        Sleep(1000);
                        system("cls");
                        cout << "========== INFORMASI PEMESANAN ===========\n" << endl;
                        cout << " Jenis kamar        : " << newReservation.roomType << endl;
                        cout << " Jenis tempat tidur : " << newReservation.bedType << endl;
                        cout << " Tanggal check-in   : " << newReservation.checkInDate << endl;
                        cout << " Tanggal check-out  : " << newReservation.checkOutDate << endl;
                        cout << " Durasi menginap    : " << numNights << " malam" << endl;
                        cout << " -----------------------------------------" << endl;
                        cout << " Nomor Kamar        : " << selectedRoomNumber << endl;
                        cout << "==========================================" << endl << endl;

                        newReservation.pembayaran = {};
                        cout << "=============== PEMBAYARAN ===============\n" << endl;
                        cout << "Pilih Metode Pembayaran:\n";
                        cout << "1. Tunai\n";
                        cout << "2. Kartu Kredit\n";
                        cout << "3. E-Wallet\n";
                        cout << "==========================================" << endl;
                        cout << "Pilihan: ";
                        int pilihanMetodePembayaran;
                        cin >> pilihanMetodePembayaran;

                        switch (pilihanMetodePembayaran) {
                            case 1:
                                newReservation.pembayaran.metodePembayaran = "Tunai";
                                break;
                            case 2:
                                {
                                    cout << endl;
                                    cout << "Pilih Bank Kartu Kredit:\n";
                                    cout << "1. BRI\n";
                                    cout << "2. BCA\n";
                                    cout << "3. Mandiri\n";
                                    cout << "4. BSI\n";
                                    cout << "Pilihan: ";

                                    int pilihanBank;
                                    cin >> pilihanBank;

                                    switch (pilihanBank) {
                                        case 1:
                                            newReservation.pembayaran.metodePembayaran = "Kartu Kredit BRI";
                                            do {
                                                cout << "Masukkan Nomor Kartu Kredit (12 digit): ";
                                                cin >> newReservation.pembayaran.nomorKartuKredit;

                                                // Validasi panjang nomor kartu kredit
                                            if (newReservation.pembayaran.nomorKartuKredit.length() != 12) {
                                                cout << "Nomor kartu kredit harus terdiri dari 12 digit." << endl;
                                            }
                                            } while (newReservation.pembayaran.nomorKartuKredit.length() != 12);
                                            break;
                                        case 2:
                                            newReservation.pembayaran.metodePembayaran = "Kartu Kredit BCA";
                                            do {
                                                cout << "Masukkan Nomor Kartu Kredit (8 digit): ";
                                                cin >> newReservation.pembayaran.nomorKartuKredit;

                                                // Validasi panjang nomor kartu kredit
                                                if (newReservation.pembayaran.nomorKartuKredit.length() != 8) {
                                                    cout << "Nomor kartu kredit harus terdiri dari 8 digit." << endl;
                                                }
                                            } while (newReservation.pembayaran.nomorKartuKredit.length() != 8);
                                            break;
                                        case 3:
                                            newReservation.pembayaran.metodePembayaran = "Kartu Kredit Mandiri";
                                            do {
                                                cout << "Masukkan Nomor Kartu Kredit (10 digit): ";
                                                cin >> newReservation.pembayaran.nomorKartuKredit;

                                                // Validasi panjang nomor kartu kredit
                                                if (newReservation.pembayaran.nomorKartuKredit.length() != 10) {
                                                    cout << "Nomor kartu kredit harus terdiri dari 10 digit." << endl;
                                                }
                                            } while (newReservation.pembayaran.nomorKartuKredit.length() != 10);
                                            break;
                                        case 4:
                                            newReservation.pembayaran.metodePembayaran = "Kartu Kredit BSI";
                                            do {
                                                cout << "Masukkan Nomor Kartu Kredit (16 digit): ";
                                                cin >> newReservation.pembayaran.nomorKartuKredit;

                                                // Validasi panjang nomor kartu kredit
                                                if (newReservation.pembayaran.nomorKartuKredit.length() != 16) {
                                                    cout << "Nomor kartu kredit harus terdiri dari 16 digit." << endl;
                                                }
                                            } while (newReservation.pembayaran.nomorKartuKredit.length() != 16);
                                            break;
                                        default:
                                            cout << "Pilihan bank kartu kredit tidak valid." << endl;
                                            exit(1); // Menghentikan program dengan kode error 1
                                    }
                                }
                                break;
                            case 3:
                                {
                                    cout << endl;
                                    cout << "Pilih E-Wallet:\n";
                                    cout << "1. DANA\n";
                                    cout << "2. GoPay\n";
                                    cout << "3. ShopeePay\n";
                                    cout << "Pilihan: ";

                                    int pilihanEWallet;
                                    cin >> pilihanEWallet;

                                    switch (pilihanEWallet) {
                                        case 1:
                                            newReservation.pembayaran.metodePembayaran = "DANA";
                                            cout << "Masukkan Email atau Nomor Handphone: ";
                                            cin >> newReservation.pembayaran.emailAtauNomorHP;
                                            break;
                                        case 2:
                                            newReservation.pembayaran.metodePembayaran = "GoPay";
                                            cout << "Masukkan Email atau Nomor Handphone: ";
                                            cin >> newReservation.pembayaran.emailAtauNomorHP;
                                            break;
                                        case 3:
                                            newReservation.pembayaran.metodePembayaran = "ShopeePay";
                                            cout << "Masukkan Email atau Nomor Handphone: ";
                                            cin >> newReservation.pembayaran.emailAtauNomorHP;
                                            break;
                                        default:
                                            cout << "Pilihan e-wallet tidak valid." << endl;
                                            return;
                                    }
                                }
                                break;
                            default:
                                cout << "Pilihan metode pembayaran tidak valid." << endl;
                                return;
                        }

                        cout << "===================================" << endl;

                       // Tampilkan informasi pembayaran
                        cout << "========== INFORMASI PEMBAYARAN ===========" << endl;
                        cout << " Metode Pembayaran   : " << newReservation.pembayaran.metodePembayaran << endl;
                        if (!newReservation.pembayaran.nomorKartuKredit.empty()) {
                            cout << " Nomor Kartu Kredit : " << newReservation.pembayaran.nomorKartuKredit << endl;
                        }
                        if (!newReservation.pembayaran.emailAtauNomorHP.empty()) {
                            cout << " Email/Nomor HP     : " << newReservation.pembayaran.emailAtauNomorHP << endl;
                        }
                        cout << "============================================" << endl;

                        // Tampilkan rekap bon
                        system("cls");
                        cout << "========== HOTEL SYER ============" << endl;
                        cout << "----------------------------------" << endl;
                        cout << " Jenis kamar           : " << newReservation.roomType << endl;
                        cout << " Jenis tempat tidur    : " << newReservation.bedType << endl;
                        cout << " Tanggal check-in      : " << newReservation.checkInDate << endl;
                        cout << " Tanggal check-out     : " << newReservation.checkOutDate << endl;
                        cout << " Durasi menginap       : " << numNights << " malam" << endl;

                        // Display Hotel Services for this reservation
                        cout << " Layanan Hotel          :\n";
                        bool hasHotelServices = false;
                        for (const auto& service : loggedInUser.hotelServices) {
                            if (service.roomNumber == newReservation.roomNumber) {
                                cout << " - " << service.serviceName << " - Harga: " << fixed << setprecision(2) << service.price << " rupiah\n";
                                hasHotelServices = true;
                            }
                        }

                        if (!hasHotelServices) {
                            cout << " Tidak ada data.\n";
                        }

                        // Display Room Facilities for this reservation
                        cout << " Fasilitas Kamar        :\n";
                        bool hasRoomFacilities = false;
                        for (const auto& facility : loggedInUser.roomFacilities) {
                            if (facility.roomNumber == newReservation.roomNumber) {
                                cout << " - " << facility.facilityName << " - Harga: " << fixed << setprecision(2) << facility.price << " rupiah\n";
                                hasRoomFacilities = true;
                            }
                        }

                        if (!hasRoomFacilities) {
                            cout << " Tidak ada data.\n";
                        }

                        cout << "----------------------------------" << endl;
                        cout << " Total harga        : Rp. " << newReservation.totalPrice * numNights << endl;
                        cout << "----------------------------------" << endl;
                        cout << " Metode Pembayaran  : " << newReservation.pembayaran.metodePembayaran << endl;
                        if (!newReservation.pembayaran.nomorKartuKredit.empty()) {
                            cout << " Nomor Kartu Kredit : " << newReservation.pembayaran.nomorKartuKredit << endl;
                        }
                        if (!newReservation.pembayaran.emailAtauNomorHP.empty()) {
                            cout << " Email/Nomor HP     : " << newReservation.pembayaran.emailAtauNomorHP << endl;
                        }
                        cout << "----------------------------------" << endl;
                        cout << "Pemesanan kamar berhasil." << endl;
                        cout << "Sedang di alihkan ke halaman awal" << endl;
                        cout << "Loading";
                        Sleep(1000);
                        cout << ".";
                        Sleep(1000);
                        cout << ".";
                        Sleep(1000);
                        cout << ".";
                        Sleep(2000);
                        system("cls");
                    } else {
                        cout << "Invalid room number. Please try again.\n";
                    }
                } else {
                    cout << "No available rooms.\n";
                }
                break;
            case '2':
                displayReservationHistory(loggedInUser);
                break;
            case '3':
                if (!loggedInUser.reservations.empty()) {
                    std::cout << std::setw(15) << "Room Number" << std::setw(20) << "Room Type"
                              << std::setw(15) << "Bed Type" << std::setw(15) << "Check-in Date"
                              << std::setw(15) << "Check-out Date" << std::setw(15) << "Total Price" << "\n";
                    std::cout << std::setfill('-') << std::setw(100) << "-" << std::setfill(' ') << "\n";
                    for (const auto& reservation : loggedInUser.reservations) {
                        std::cout << std::setw(15) << reservation.roomNumber
                          << std::setw(20) << reservation.roomType
                          << std::setw(15) << reservation.bedType
                          << std::setw(15) << reservation.checkInDate
                          << std::setw(15) << reservation.checkOutDate
                          << std::setw(15) << std::fixed << std::setprecision(2) << reservation.totalPrice << "\n";
                    }
                    cout << "\n\n\nKembali he halaman sebelumnya [y]?";
                    cin >> serviceChoice;
                    switch(serviceChoice) {
                        case 'y':
                        case 'Y':
                            system("cls");
                            break;
                        default:
                            system("cls");
                    }
                } else {
                    cout << "\nNo booking history found.\n\n";
                    cout << "Loading";
                    Sleep(1000);
                    cout << ".";
                    Sleep(1000);
                    cout << ".";
                    Sleep(1000);
                    cout << ".";
                    Sleep(1500);
                    system("cls");
                }
                break;
            case '4':
                layananHotel(ordersHotel);
                break;
            case '5':
                layananKamar(ordersKamar); // Implement this function
                break;
            case '6':
                evaluateHotel(loggedInUser);
                break;
            case '7':
                cout << "Logging out.\n";
                cout << "Loading";
                Sleep(1000);
                cout << ".";
                Sleep(1000);
                cout << ".";
                Sleep(1000);
                cout << ".";
                Sleep(1500);
                system("cls");

                return;
            default:
                cout << "Please try again.\n";
        }
    } while (serviceChoice != '6');
}

void saveUserDataToFile() {
    ofstream file("users.txt");
    if (file.is_open()) {
        for (const auto& user : users) {
            file << user.firstName << " "
                 << user.lastName << " "
                 << user.phoneNumber << " "
                 << user.email << " "
                 << user.username << " "
                 << user.password << "\n";
            file << user.availableRooms.size() << "\n";

            for (const auto& room : user.availableRooms) {
                file << room.roomNumber << " "
                     << room.roomType << " "
                     << room.bedType << " "
                     << room.pricePerNight << "\n";
            }
        }
        file.close();
        cout << "User data saved to file.\n";
    } else {
        cout << "Unable to open file for saving user data.\n";
    }
}

void loadUserDataFromFile() {
    ifstream file("users.txt");
    if (file.is_open()) {
        users.clear(); // Clear existing user data
        User user;
        while (file >> user.firstName >> user.lastName >> user.phoneNumber >> user.email >> user.username >> user.password) {
            int numRooms;
            file >> numRooms;

            // Initialize available rooms randomly
            user.availableRooms.clear();
            for (int i = 0; i < numRooms; ++i) {
                Room newRoom;
                newRoom.roomNumber = generateRandomRoomNumber();
                file >> newRoom.roomType >> newRoom.bedType >> newRoom.pricePerNight;
                user.availableRooms.push_back(newRoom);
            }

            users.push_back(user);
        }
        file.close();
        cout << "User data loaded from file.\n";
    } else {
        cout << "Unable to open file for loading user data.\n";
    }
}


int main() {
    loadUserDataFromFile(); // Load user data from file when the program starts
    char choice;
    User loggedInUser;
    do {
        system("cls");
        cout << "SELAMAT DATANG\n";
        cout << "--------------------------------------------------\n\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n\n";
        cout << "--------------------------------------------------\n";
        cout << "Pilihanmu          : ";
        cin >> choice;
        system("cls");

        switch (choice) {
            case '1':
                registerUser();
                break;
            case '2':
                if (loginUser(loggedInUser)) {
                    showHotelServices(loggedInUser);
                }
                break;
            case '3':
                cout << "Loading";
                Sleep(500);
                cout << ".";
                Sleep(1000);
                cout << ".";
                Sleep(1000);
                cout << ".";
                Sleep(1000);
                system("cls");
                saveUserDataToFile(); // Save user data to file before exiting
                cout << "Exiting program.\n";
                system("cls");
                cout << "\033[2J\033[H";
                cout << "\033[38;2;255;165;0m";  // Warna orange (RGB: 255, 165, 0)
                cout << "\033[6m";
                cout << "--------------------------------------------------\n";
                cout << "                   SAMPAI JUMPA                   \n";
                cout << "--------------------------------------------------\n";
                cout << "\033[0m";
                cout << endl;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != '3');

    return 0;
}
