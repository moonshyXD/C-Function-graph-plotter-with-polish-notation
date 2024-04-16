#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define INIT_SIZE 10
#define MULTIPLIER 2
#define STACK_OVERFLOW -100
#define STACK_UNDERFLOW -101
#define OUT_OF_MEMORY -102
#define M_PI 3.14159265358979323846
#define WIDTH 80
#define HEIGHT 25
typedef char* T;

typedef struct Stack_tag {
    T* data;     
    size_t size; 
    size_t top;   
} Stack_T;


Stack_T* createStack() {
    Stack_T* out = (Stack_T*)malloc(sizeof(Stack_T));
    if (out == NULL) {
        exit(OUT_OF_MEMORY); 
    }
    out->size = 1; 
    out->data = (T*)malloc(out->size * sizeof(T)); 
    if (out->data == NULL) {
        free(out);
        exit(OUT_OF_MEMORY);
    }
    out->top = 0; 
    return out;
}

void deleteStack(Stack_T** stack) {
    if (*stack != NULL) {
        free((*stack)->data);
        free(*stack); 
        *stack = NULL;
    }
}

void resize(Stack_T* stack) {
    stack->size *= MULTIPLIER;  
    stack->data = (T*)realloc(stack->data, stack->size * sizeof(T)); 
    if (stack->data == NULL) {
        exit(OUT_OF_MEMORY);
    }
}

void push(Stack_T* stack, T value) {
    if (stack->top >= stack->size) {
        resize(stack); 
    }
    stack->data[stack->top] = value; 
}

Stack_T* copyStack(Stack_T* original) {
    Stack_T* copy = createStack();
    for (size_t i = 0; i < original->top; i++) {
        push(copy, original->data[i]);
    }
    return copy;
}


T pop(Stack_T* stack) {
    if (stack->top == 0) {
        exit(STACK_UNDERFLOW);  
    }
    stack->top--;  
    return stack->data[stack->top];  
}

T peek(const Stack_T* stack) {
    if (stack->top == 0) {
        exit(STACK_UNDERFLOW);
    }
    return stack->data[stack->top - 1]; 
}


int is_operator(char* op) {
    return (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "/") == 0 || strcmp(op, "*") == 0);
}

int is_function1(char* value) {
    return (strcmp(value, "sin") == 0 || strcmp(value, "cos") == 0 || strcmp(value, "tan") == 0 ||
        strcmp(value, "ctg") == 0 || strcmp(value, "ln") == 0 || strcmp(value, "sqrt") == 0);;
}


int is_number(char* token) {
    while (*token) {
        if (!isdigit(*token)) {
            return 0;
        }
        token++; 
    }
    return 1;
}

int is_variable(char* value) {
    while (*value) {
        if (!isalpha(*value)) { 
            return 0; 
        }
        value++; 
    }
    return 1; 
}

int get_precedence(char* op) {
    if (strcmp(op, "sin") == 0 || strcmp(op, "cos") == 0 || strcmp(op, "tan") == 0 ||
        strcmp(op, "ctg") == 0 || strcmp(op, "ln") == 0 || strcmp(op, "sqrt") == 0) return 3;
    if (strcmp(op, "/") == 0 || strcmp(op, "*") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "x") == 0) return 1;
    return 0;
}


char* apply_operator(Stack_T* operands, char* _operator) {
    double result = 0.0;
    double a, b;
    if (is_function1(_operator)) {
        a = atof(peek(operands));
        pop(operands);
    }
    else {
        a = atof(peek(operands));
        pop(operands);
        b = atof(peek(operands));
        pop(operands);
        double tmp = a;
        a = b;
        b = tmp;
    }

    if (is_function1(_operator)) {
        if (strcmp(_operator, "sin") == 0) {
            result = sin(a);
        }
        else if (strcmp(_operator, "cos") == 0) {
            result = cos(a);
        }
        else if (strcmp(_operator, "tan") == 0) {
            result = tan(a);
        }
        else if (strcmp(_operator, "ctg") == 0) {
            result = 1 / tan(a);
        }
        else if (strcmp(_operator, "sqrt") == 0) {
            result = sqrt(a);
        }
        else if (strcmp(_operator, "ln") == 0) {
            result = log(a);
        }
    }
    else {
        // Âû÷èñëåíèå ðåçóëüòàòà â çàâèñèìîñòè îò îïåðàòîðà
        if (strcmp(_operator, "+") == 0) {
            result = a + b;
        }
        else if (strcmp(_operator, "-") == 0) {
            result = a - b;
        }
        else if (strcmp(_operator, "*") == 0) {
            result = a * b;
        }
        else if (strcmp(_operator, "/") == 0) {
            if (b == 0) {
                printf("Error: Division by zero\nPlease, enter 'x' like a first operand\n");
                exit(EXIT_FAILURE);
            }
            result = a / b;
        }
    }
    char* output = (char*)malloc(sizeof(char*) * 1000);
    sprintf_s(output, strlen(output), "%f", result);

    return output; 
}

Stack_T* tokenize(const char* expression) {
    Stack_T* tokens = createStack();
    char finalStr[15] = "";
    char* str = (char*)malloc(5);
    for (int i = 0; i < strlen(expression); i++)
    {
        char* token = expression[i];
        if (isspace(expression[i])) {
            if (finalStr != "") {
                strcpy_s(str, sizeof(str), finalStr);
                push(tokens, str);
                memset(finalStr, 0, sizeof(finalStr));
                str = (char*)malloc(5);
            }
        }
        else if (isdigit(expression[i]) || isalpha(expression[i]) || expression[i] == '.') {
            strncat_s(finalStr, sizeof(finalStr), &expression[i], 1);
        }
        else {
            if (finalStr != "") {
                strcpy_s(str, sizeof(str), finalStr);
                push(tokens, str);
                memset(finalStr, 0, sizeof(finalStr));
                str = (char*)malloc(5);
            }
            strcpy_s(str, sizeof(str), &token);
            push(tokens, str);
            str = (char*)malloc(5);
        }
    }
    if (finalStr != "") {
        strcpy_s(str, sizeof(str), finalStr);
        push(tokens, str);
        memset(finalStr, 0, sizeof(finalStr));
    }

    Stack_T* cout = copyStack(tokens);
    while (cout->top) {
        printf("%s", pop(cout));
    }
    printf("\n");

    return tokens;
}


Stack_T* replace_x_with_Number(Stack_T* expression_tokens, double x_value) {
    char* x_str = (char*)malloc(10);
    Stack_T* output = createStack();
    while (expression_tokens->top) {
        char* tmp = peek(expression_tokens);
        if (strcmp(peek(expression_tokens), "x") == 0) {
            sprintf_s(x_str, strlen(x_str), "%f", x_value);
            char* tmp = (char*)malloc(10);
            strcpy_s(tmp, strlen(tmp), x_str);
            push(output, tmp);
            pop(expression_tokens);
            memset(x_str, 0, sizeof(x_str));
            x_str = (char*)malloc(10);
        }
        else {
            if (!strcmp(tmp, "") == 0) {
                push(output, tmp);
            }
            pop(expression_tokens);
        }
    }
    free(x_str);

    Stack_T* cout = copyStack(output);
    while (cout->top) {
        printf("%s", peek(cout));
        printf("%s", " ");
        pop(cout);
    }
    printf("\n");
    return output;
}

Stack_T* shunting_yard(const char* expression) {
    Stack_T* output_queue = createStack();
    Stack_T* operator_stack = createStack();
    Stack_T* tokens = tokenize(expression);

    Stack_T* reverse_tokens = createStack();
    while (tokens->top) {
        push(reverse_tokens, peek(tokens));
        pop(tokens);
    }

    while (reverse_tokens->top) {
        char* token = peek(reverse_tokens);
        if (!strcmp(token, "") == 0) {
            if (is_number(token) || is_variable(token)) {
                push(output_queue, token);
            }
            else if (is_function1(token)) {
                push(output_queue, token);
            }
            else if (is_operator(token)) {
                while (operator_stack->top && get_precedence(peek(operator_stack)) >= get_precedence(token)) {
                    push(output_queue, peek(operator_stack));
                    pop(operator_stack);
                }
                push(operator_stack, token);
            }
        }
        pop(reverse_tokens);
    }
    while (operator_stack->top) {
        push(output_queue, peek(operator_stack));
        pop(operator_stack);
    }
    deleteStack(&operator_stack);
    deleteStack(&tokens);

    Stack_T* cout = copyStack(output_queue);
    while (cout->top) {
        printf("%s", peek(cout));
        pop(cout);
    }


    printf("\n");
    return output_queue;
}

double evaluate_polish_notation(Stack_T* expression) {
    Stack_T* operands = createStack();
    while (expression->top) {
        char* token = peek(expression);
        if (is_operator(token)) {
            if (operands->top < 2) {
                pop(expression);
                push(operands, peek(expression));
            }
            push(operands, apply_operator(operands, token));
        }
        else if (is_function1(token)) {
            if (operands->size < 1) {
                printf("Error");
                exit(STACK_UNDERFLOW);
            }
            pop(expression);
            push(operands, peek(expression));
            push(operands, apply_operator(operands, token));
        }
        else {
            push(operands, token);
        }
        pop(expression);
    }

    if (operands->top != 1) {
        printf("Error");
        exit(STACK_UNDERFLOW);
    }
    double output = atof(peek(operands));
    printf("%f", output);
    printf("\n");
    return output;
}

void plot_graph(Stack_T* expression, int width, int height) {
    char graph[HEIGHT][WIDTH];
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            graph[i][j] = '.';
        }
    }
    int middle_y = HEIGHT / 2, y;
    double val, real_x;
    for (int x = 0; x < width; x++)
    {
        double x2 = x;
        real_x = (x2 / (double)WIDTH) * (4 * M_PI);
        Stack_T* tmp = copyStack(expression);
        Stack_T* new_expression = replace_x_with_Number(tmp, real_x);
        val = evaluate_polish_notation(new_expression);

        y = (val + real_x) * (middle_y - real_x) / 3;
        if (y >= 0 && y < HEIGHT) {
            graph[HEIGHT - y - 1][x] = '*';
        }
    }

    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            printf("%c", graph[i][j]);
        }
        printf("\n");
    }
}

int main() {
    const char* expression = "sin(30) + x - cos(45) * 2 / 3";
    Stack_T* parsed_expression = createStack();
    parsed_expression = shunting_yard(expression);
    plot_graph(parsed_expression, WIDTH, HEIGHT);

    return 0;
}
