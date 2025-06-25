/*
=========================================
   Interactive Terminal Calculator - C
=========================================

Supports:
- Full expression parsing: e.g., (3 + 4) * 2 - 1
- Operator precedence: +, -, *, /, %, ^ with parentheses
- Unary functions: sqrt, abs, log, ln, exp, fact, sin, cos, tan
- Special: Ans (last result), 'c' to clear screen, 'q' to quit

Author: Vaggelis Papaioannou
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#define MAX_TOKENS 100
#define MAX_STACK 100

typedef enum { NUMBER, OPERATOR, FUNCTION, PAREN_LEFT, PAREN_RIGHT } TokenType;

typedef struct {
    TokenType type;
    double value;   // if NUMBER
    char op;        // if OPERATOR
    char func[10];  // if FUNCTION
} Token;

// Function prototypes
double evaluate_expression(const char *expr, double last_result, int *error);
int precedence(char op);
int is_right_associative(char op);
int is_function(const char *s);
double apply_operator(char op, double a, double b, int *error);
double apply_function(const char *func, double a, int *error);
double factorial(int n);
double degrees_to_radians(double deg);

// Tokenization

//Tokenization is the process of breaking down a math expression (like "3 + sqrt(4 * 2)") into individual elements, or tokens, that the calculator
// can understand and manipulate programmatically.

int tokenize(const char *expr, Token tokens[], double last_result) {
    int count = 0;
    const char *p = expr;

    while (*p) {
        if (isspace(*p)) { p++; continue; }

        if (isdigit(*p) || (*p == '.' && isdigit(*(p+1)))) {
            sscanf(p, "%lf", &tokens[count].value);
            tokens[count].type = NUMBER;
            while (isdigit(*p) || *p == '.') p++;
            count++;
        } else if (isalpha(*p)) {
            char func[10] = {0};
            int i = 0;
            while (isalpha(*p)) func[i++] = *p++;
            func[i] = '\0';

            if (strcmp(func, "Ans") == 0) {
                tokens[count].type = NUMBER;
                tokens[count].value = last_result;
                count++;
            } else {
                tokens[count].type = FUNCTION;
                strcpy(tokens[count].func, func);
                count++;
            }
        } else if (*p == '(') {
            tokens[count++].type = PAREN_LEFT;
            p++;
        } else if (*p == ')') {
            tokens[count++].type = PAREN_RIGHT;
            p++;
        } else if (strchr("+-*/%^", *p)) {
            tokens[count].type = OPERATOR;
            tokens[count++].op = *p++;
        } else {
            return -1; // invalid token
        }
    }

    return count;
}

// Shunting Yard: Infix to Postfix

/*

This part of the code uses the Shunting Yard algorithm, invented by Edsger Dijkstra, 
to convert a math expression written in infix notation (what humans write, like 3 + 4 * 2) 
into postfix notation (what machines evaluate more easily, like 3 4 2 * +).

*/
int to_postfix(Token in[], int n, Token out[]) {
    Token stack[MAX_STACK];
    int out_i = 0, stack_i = 0;

    for (int i = 0; i < n; i++) {
        Token t = in[i];
        if (t.type == NUMBER) {
            out[out_i++] = t;
        } else if (t.type == FUNCTION) {
            stack[stack_i++] = t;
        } else if (t.type == OPERATOR) {
            while (stack_i > 0 && (
                (stack[stack_i - 1].type == FUNCTION) ||
                (stack[stack_i - 1].type == OPERATOR &&
                 ((precedence(stack[stack_i - 1].op) > precedence(t.op)) ||
                 (precedence(stack[stack_i - 1].op) == precedence(t.op) &&
                  !is_right_associative(t.op))))
            )) {
                out[out_i++] = stack[--stack_i];
            }
            stack[stack_i++] = t;
        } else if (t.type == PAREN_LEFT) {
            stack[stack_i++] = t;
        } else if (t.type == PAREN_RIGHT) {
            while (stack_i > 0 && stack[stack_i - 1].type != PAREN_LEFT) {
                out[out_i++] = stack[--stack_i];
            }
            if (stack_i == 0) return -1; // Mismatched parentheses
            stack_i--; // Pop '('

            if (stack_i > 0 && stack[stack_i - 1].type == FUNCTION)
                out[out_i++] = stack[--stack_i];
        }
    }

    while (stack_i > 0) {
        if (stack[stack_i - 1].type == PAREN_LEFT) return -1;
        out[out_i++] = stack[--stack_i];
    }

    return out_i;
}

// Evaluate Postfix
double eval_postfix(Token tokens[], int n, int *error) {
    double stack[MAX_STACK];
    int top = 0;

    for (int i = 0; i < n; i++) {
        Token t = tokens[i];
        if (t.type == NUMBER) {
            stack[top++] = t.value;
        } else if (t.type == OPERATOR) {
            if (top < 2) { *error = 1; return 0; }
            double b = stack[--top];
            double a = stack[--top];
            stack[top++] = apply_operator(t.op, a, b, error);
        } else if (t.type == FUNCTION) {
            if (top < 1) { *error = 1; return 0; }
            double a = stack[--top];
            stack[top++] = apply_function(t.func, a, error);
        }
    }

    if (top != 1) { *error = 1; return 0; }
    return stack[0];
}

// Expression evaluator
double evaluate_expression(const char *expr, double last_result, int *error) {
    Token tokens[MAX_TOKENS], postfix[MAX_TOKENS];
    int ntokens = tokenize(expr, tokens, last_result);
    if (ntokens < 0) { *error = 1; return 0; }

    int npost = to_postfix(tokens, ntokens, postfix);
    if (npost < 0) { *error = 1; return 0; }

    return eval_postfix(postfix, npost, error);
}

// Helpers
int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': case '%': return 2;
        case '^': return 3;
        default: return 0;
    }
}
int is_right_associative(char op) {
    return op == '^';
}
double apply_operator(char op, double a, double b, int *error) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': if (b == 0) { *error = 1; return 0; } return a / b;
        case '%': if ((int)b == 0) { *error = 1; return 0; } return (int)a % (int)b;
        case '^': return pow(a, b);
        default: *error = 1; return 0;
    }
}
double apply_function(const char *func, double a, int *error) {
    if (strcmp(func, "sqrt") == 0) return a < 0 ? (*error = 1, 0) : sqrt(a);
    if (strcmp(func, "abs") == 0) return fabs(a);
    if (strcmp(func, "ln") == 0) return a <= 0 ? (*error = 1, 0) : log(a);
    if (strcmp(func, "log") == 0) return a <= 0 ? (*error = 1, 0) : log10(a);
    if (strcmp(func, "exp") == 0) return exp(a);
    if (strcmp(func, "fact") == 0) return (a < 0 || floor(a) != a) ? (*error = 1, 0) : factorial((int)a);
    if (strcmp(func, "sin") == 0) return sin(degrees_to_radians(a));
    if (strcmp(func, "cos") == 0) return cos(degrees_to_radians(a));
    if (strcmp(func, "tan") == 0) return tan(degrees_to_radians(a));
    *error = 1;
    return 0;
}
double factorial(int n) {
    if (n < 0) return 0;
    double res = 1;
    for (int i = 2; i <= n; i++) res *= i;
    return res;
}
double degrees_to_radians(double deg) {
    return deg * M_PI / 180.0;
}

// Main program
int main() {
    char input[256];
    double result = 0.0, last_result = 0.0;
    int error;

    printf("=== Terminal Calculator ===\n");
    printf("Supports full expressions (e.g., (3 + 2) * 5 - 1 / 2)\n");
    printf("Unary functions: sqrt, log, sin, fact, etc. | Use 'Ans' for last result\n");
    printf("Type 'q' to quit, 'c' to clear screen.\n");

    while (1) {
        printf("\nEnter expression: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "q") == 0 || strcmp(input, "Q") == 0) {
            printf("Goodbye!\n");
            break;
        }
        if (strcmp(input, "c") == 0 || strcmp(input, "C") == 0) {
            system("cls"); // Windows clear
            continue;
        }

        error = 0;
        result = evaluate_expression(input, last_result, &error);
        if (error)
            printf("Error: Invalid expression\n");
        else {
            printf("Result: %.6lf\n", result);
            last_result = result;
        }
    }

    return 0;
}
