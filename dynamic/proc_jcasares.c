#include "../rpc.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    reportable_t parent;
    int *data;
    int size;
    double mean;
    int mode;
    double median;
} my_struct_t;

void *parse_parameters(void *data)
{
    const char *buf = (const char *)(data);

    my_struct_t *d = (my_struct_t *)(malloc(sizeof(my_struct_t)));

    if (d)
    {
        // Paso 1: Leer el tamaño del arreglo
        sscanf(buf, "%d", &d->size);

        // Paso 2: Asignar memoria para almacenar los datos
        d->data = (int *)(malloc(d->size * sizeof(int)));

        // Paso 3: Leer los datos del arreglo
        for (int i = 0; i < d->size; i++)
        {
            sscanf(buf, "%d", &d->data[i]);
        }
    }

    return (void *)d;
}

void *do_work(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);

    // Paso 4: Calcular la media
    double sum = 0.0;
    for (int i = 0; i < d->size; i++)
    {
        sum += d->data[i];
    }
    d->mean = sum / d->size;

    // Paso 5: Calcular la moda
    int *count = (int *)(calloc(10, sizeof(int))); // Contador para cada dígito del 0 al 9

    for (int i = 0; i < d->size; i++)
    {
        count[d->data[i]]++;
    }

    int maxCount = 0;
    for (int i = 0; i < 10; i++)
    {
        if (count[i] > maxCount)
        {
            maxCount = count[i];
            d->mode = i;
        }
    }
    free(count);

    // Paso 6: Calcular la mediana
    int *sortedData = (int *)(malloc(d->size * sizeof(int)));
    memcpy(sortedData, d->data, d->size * sizeof(int));
    qsort(sortedData, d->size, sizeof(int), compare);

    if (d->size % 2 == 0)
    {
        d->median = (sortedData[d->size / 2 - 1] + sortedData[d->size / 2]) / 2.0;
    }
    else
    {
        d->median = sortedData[d->size / 2];
    }
    free(sortedData);

    return data;
}

reportable_t *report(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);

    d->parent.data = (char *)(malloc(255 * sizeof(char)));

    // Paso 7: Generar el informe con los resultados obtenidos
    snprintf(d->parent.data, 255, "Mean: %.2f\nMode: %d\nMedian: %.2f\n", d->mean, d->mode, d->median);
    d->parent.len = strlen(d->parent.data);

    return (reportable_t *)(data);
}

void clean_up(void *params, void *result, reportable_t *report)
{
    if (report && report->data)
    {
        free(report->data);
    }

    if (params)
    {
        my_struct_t *d = (my_struct_t *)(params);
        free(d->data);
        free(params);
    }
}

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

