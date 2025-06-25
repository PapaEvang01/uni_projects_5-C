# C Projects – My First Steps in Programming

These are the first C projects I developed during my **first year at the Department of Electrical and Computer Engineering, Democritus University of Thrace**.  
Each of these small terminal-based programs helped me understand the **fundamentals of programming**, including logic building, structured design, data handling, and memory operations.

---

##  Terminal Calculator in C

A fully interactive **command-line calculator** that parses and evaluates complete mathematical expressions using the **Shunting Yard Algorithm**.

###  Features:
- Supports all basic operators: `+`, `-`, `*`, `/`, `%`, `^`
- Parentheses and correct **operator precedence**
- Built-in functions: `sqrt`, `abs`, `log`, `ln`, `exp`, `fact`, `sin`, `cos`, `tan`
- "Ans" keyword to reuse the last result
- Terminal commands:
  - `q` → quit
  - `c` → clear

📌 **What I Learned:**
- Stack-based expression parsing
- Implementing mathematical logic
- Turning user input into real-time computation

📁 Files:
- `calculator.c` (single file)

---

##  Student Record Management System

A terminal-based program for managing a small database of students using binary file storage.

###  Features:
- Add student (first name, last name, roll number)
- View all students
- Search or delete a specific student
- Export student list to `.csv` for Excel
- Data is saved in `students.dat` (binary file) for persistence

📌 **What I Learned:**
- File I/O operations in C
- Working with `struct` data types
- Building a functional CRUD application

📁 Files:
- `student_records.c`

---

##  To-Do List Manager (C Terminal App)

A feature-rich to-do list program with task management and filtering features — all within the terminal.

###  Features:
- Add tasks with:
  - Title & Description
  - Priority level
  - Category (Work, Study, Personal, Other)
  - Deadline (date-based)
- Sort tasks by deadline automatically
- Mark tasks as completed
- View tasks filtered by category
- Save and load data from file
- **Color-coded terminal output** for readability

📌 **What I Learned:**
- Real-time user input and file persistence
- Sorting by custom criteria (deadline)
- Date comparisons and terminal UX in plain C

📁 Files:
- `todo_list.c`

---

## 🔧 Core Concepts Practiced

- C language fundamentals
- File handling with binary and text formats
- Stack and expression evaluation
- CRUD systems and struct manipulation
- Sorting and filtering with dates
- Terminal UI and user experience design

---

## 👨‍💻 Author

**Evageloss Papaioannou**  
Department of Electrical & Computer Engineering  
Democritus University of Thrace
