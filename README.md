
# No Due Fees Management System (C)

A **terminal-based Student No-Due / Approval Management System** written in **C**, focused on core system logic, file handling, and role-based access control.

This project manages student records, pending approvals, and admin verification using **persistent file storage** instead of databases or GUIs, making it lightweight, portable, and easy to test.

---
---

## 🖼️ Demo Screenshot

Below is a sample screenshot of the terminal-based No Due Fees Management System in action:

![Demo Screenshot](Demo/Demo_Image.JPG)



---

## 📌 Features

### 👨‍🎓 Student Portal
- View approval (No-Due) status
- Apply for approval if pending
- Duplicate approval requests are prevented

### 🧑‍💼 Admin Portal
- Secure admin login (username + masked password)
- View all student records
- View pending approval requests
- Approve / reject / skip applications
- Update student dues and approval status
- Add new student records

### 💾 Data Persistence
All data is stored in plain-text files:
- Student records persist across program runs
- Approval requests are queued and processed safely

---

## 🗂 File Structure

```

No Due fees management system/
│── main.c                # Main source code
│── student.txt           # Student database (CSV format)
│── approval_list.txt     # Pending approval requests
│── payment_history.txt   # Payment log (future extensibility)
│── .gitignore            # Git ignore rules

````

No binaries or temporary files are tracked in the repository.

---

## ⚙️ How to Compile and Run

### Compile
```bash
gcc main.c -o main
````

### Run

```bash
./main        # Linux / macOS
main.exe      # Windows
```

---

## 🔐 Admin Credentials (default)

```
Username: admin
Password: admin123
```

> ⚠️ Note: Credentials are hardcoded for academic/demo purposes only.

---

## 🧪 Testing Approach

* The application is fully testable via **standard input redirection**
* Menus and logic can be automated using input files
* Password masking uses `getch()` and therefore requires real keyboard input during admin login (expected behavior)

This project prioritizes **logic correctness and persistence** over UI.

---

## 🎯 Design Focus

* Modular functions (single responsibility)
* Clear separation of student/admin workflows
* Defensive input validation
* Safe file handling using temporary files during updates
* Easily extendable to:

  * GUI
  * Database (SQLite/MySQL)
  * Web backend

---

## 🚀 Future Improvements

* GUI or web-based frontend
* Encrypted credential storage
* Role-based permissions
* Database-backed storage
* Detailed payment tracking reports

---

## 📚 Academic Note

This project was developed to strengthen understanding of:

* C programming
* File handling
* Structures
* Control flow
* System design fundamentals

The terminal-based interface is **intentional** to emphasize backend logic and reliability.

---

## 👤 Author

**Aditya Bhardwaj**
B.Tech – Computer Science & Engineering

---

## 📄 License

This project is intended for educational use.

```

---

### 🧠 Final honest take  
This README:
- doesn’t oversell  
- doesn’t undersell  
- explains **why** it’s terminal-based  
- sounds intentional and confident  





