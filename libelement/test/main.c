#include "element/token.h"
#include "element/ast.h"
#include "element/interpreter.h"

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

int main(int argc, char** argv)
{
    //todo: good location for these + copy on build
    //char* packages[] = {
    //    "../test/packages/test/"
    //};

    //char* files[] = {
    //    "../test/test.ele",
    //    "../test/test - Copy.ele"
    //};

    element_interpreter_ctx* ictx;
    element_interpreter_create(&ictx);
    element_interpreter_load_prelude(ictx);

    const char* evaluate = "fakeadd(a:Num, b:Num) = Num.add(a, b);";
    auto result = element_interpreter_load_string(ictx, evaluate, "<input>");
    if (result != ELEMENT_OK)
        return result;

    const element_function* fn;

    result = element_interpreter_get_function(ictx, "fakeadd", &fn);
    if (result != ELEMENT_OK)
        return result;

    element_compiled_function* cfn;

    result = element_interpreter_compile_function(ictx, fn, &cfn, NULL);
    if (result != ELEMENT_OK)
        return result;

    element_value inputs[2] = { 1, 2 };
    element_value outputs[1];

    result = element_interpreter_evaluate_function(ictx, cfn, inputs, 2, outputs, 1, NULL);
    if (result != ELEMENT_OK)
        return result;

    printf("%f", outputs[0]);

    //element_result result = element_interpreter_load_files(ictx, files, 1);
    //element_interpreter_load_packages(ictx, packages, 1);

    element_interpreter_delete(ictx);

    return 0;

    //Old Code
    /*

    const char* input =
        "Potato1(n:num) : num { return = thing.otherthing; } \n"
        "Potato2(n:num) : num { return = thing(x).otherthing; } \n"
        "Potato3(n:num) : num { return = thing(x).otherthing(y); } \n"
        "Potato4(n:num) : num { return = thing(x).otherthing.another(z).what; } \n"
        "Tomato(n:num) = 10.mul(n); \n"
        "Tomato2 = 5; \n"
        "Tomato4(n:num) = Tomato(n).add(Tomato(5)).add(n);";

    printf("Input\n\n%s\n", input);

    element_tokeniser_ctx* tctx;
    element_tokeniser_create(&tctx);

    element_interpreter_ctx* ictx;
    element_interpreter_create(&ictx);

    clock_t start = clock();
    const size_t iters = 100;*/

/*
    for (size_t i = 0; i < iters; ++i) {
        // element_interpreter_load_string(ictx, input, "<input>");
        // element_interpreter_clear(ictx);

        element_tokeniser_run(tctx, input, "<input>");
        element_tokeniser_clear(tctx);
    }
*/

    /*for (size_t i = 0; i < iters; ++i) {
        for (int j = 1; j < argc; ++j) {
            // printf("%s: ", argv[i]);

            FILE* f = fopen(argv[j], "rb");
            fseek(f, 0, SEEK_END);
            long pos = ftell(f);
            fseek(f, 0, SEEK_SET);

            char* buf = calloc(pos + 1, sizeof(char));
            fread(buf, sizeof(char), pos, f);
            fclose(f);
            buf[pos] = '\0';

            element_interpreter_load_string(ictx, buf, argv[j]);
            // printf("OK\n");
            free(buf);
        }

        element_interpreter_load_string(ictx, input, "<input>");
        if (i + 1 < iters)
            element_interpreter_clear(ictx);
    }


    clock_t end = clock();
    double t = 1000000.0 * ((double)(end - start) / CLOCKS_PER_SEC) / iters;
    printf("time (us): %lf\n", t);

    const element_function* fn = NULL;
    element_compiled_function* cfn = NULL;
    element_value inputs[2] = { 3.0, 4.0 };
    element_value outputs[1];
    element_interpreter_get_function(ictx, "num.add", &fn);
    element_interpreter_compile_function(ictx, fn, &cfn, NULL);
    element_interpreter_evaluate_function(ictx, cfn, inputs, 2, outputs, 1, NULL);
    printf("add(%f,%f) = %f\n", inputs[0], inputs[1], outputs[0]);

    inputs[0] = 3.14159f / 4.0f;
    element_interpreter_get_function(ictx, "num.sin", &fn);
    element_interpreter_compile_function(ictx, fn, &cfn, NULL);
    element_interpreter_evaluate_function(ictx, cfn, inputs, 1, outputs, 1, NULL);
    printf("sin(%f) = %f\n", inputs[0], outputs[0]);

    inputs[0] = 2.5f;
    element_interpreter_get_function(ictx, "Tomato", &fn);
    element_interpreter_compile_function(ictx, fn, &cfn, NULL);
    element_interpreter_evaluate_function(ictx, cfn, inputs, 1, outputs, 1, NULL);
    printf("Tomato(%f) = %f\n", inputs[0], outputs[0]);

    inputs[0] = 2.5f;
    element_interpreter_get_function(ictx, "Tomato4", &fn);
    element_interpreter_compile_function(ictx, fn, &cfn, NULL);
    element_interpreter_evaluate_function(ictx, cfn, inputs, 1, outputs, 1, NULL);
    printf("Tomato4(%f) = %f\n", inputs[0], outputs[0]);

    element_interpreter_delete(ictx);
    
    getchar();
    return 0;*/
}