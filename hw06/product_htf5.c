#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <hdf5.h>
#include "ini.h"

typedef struct {
    int len;
    int chu;
    double a;
    double x;
    double y;
} Config;


void writeFloatHDF5(float f, hid_t file, const char* name)
{
    hid_t scalar = H5Screate(H5S_SCALAR);            // scalar dataspace
    hid_t dset3  = H5Dcreate2(file, name, H5T_NATIVE_FLOAT, scalar,
                              H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dset3, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &f);
    H5Dclose(dset3);
    H5Sclose(scalar);

    return;
};

void writeFloatArrayHDF5(float* f, int size, hid_t file, const char* name)
{
    hsize_t dims1[1] = {size};                          // length 5
    hid_t space1 = H5Screate_simple(1, dims1, NULL); // 1D dataspace
    hid_t dset1  = H5Dcreate2(file, name, H5T_NATIVE_FLOAT, space1,
                              H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dset1, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, f);
    H5Dclose(dset1);
    H5Sclose(space1);

    return;
};


static int handler(void *user, const char *section, const char *name, const char *value)
{
    (void)section;
    Config *cfg = (Config *)user;
 
    char *end;
    errno = 0;
    double v = strtod(value, &end);
    if (end == value || errno != 0) {
        fprintf(stderr, "Warning: invalid value for '%s': %s\n", name, value);
        return 1;
    }
 
    /* Map each name to the right field */
    if(strcmp(name, "len") == 0) cfg->len = v;
    else if(strcmp(name, "chu") == 0) cfg->chu = v;
    else if(strcmp(name, "a") == 0) cfg->a = v;
    else if(strcmp(name, "x") == 0) cfg->x = v;
    else if(strcmp(name, "y") == 0) cfg->y = v;
    else fprintf(stderr, "Warning: unknown parameter '%s' — ignored.\n", name);
 
    return 1;
}


int main(int argc, char *argv[])
{
    const char *filepath = (argc > 1) ? argv[1] : "params.txt";
    const char *fileout = (argc > 2) ? argv[2] : "result.h5";
 
    /* Default values (in case some params are missing from the file) */
    Config cfg = {
        .len = 10,
        .chu = 2,
        .a = 12.2,
        .x = 1.1,
        .y = 2.2,
    };
 
    int rc = ini_parse(filepath, handler, &cfg);
    if (rc == -1) { fprintf(stderr, "Cannot open '%s'\n", filepath); return 1; }
    if (rc  >  0) { fprintf(stderr, "Parse error on line %d\n", rc); return 1; }

    printf("Loaded parameters:\n");
    printf("  len = %d\n", cfg.len);
    printf("  chu = %d\n", cfg.chu);
    printf("  a   = %.2f\n", cfg.a);
    printf("  x   = %.2f\n", cfg.x);
    printf("  y   = %.2f\n\n", cfg.y);

    float* x = malloc(cfg.len * sizeof(float));
    float* y = malloc(cfg.len * sizeof(float));
    float* r = malloc(cfg.len * sizeof(float));

    for(int i = 0; i < cfg.len; i++)
    {
        x[i] = cfg.x;
        y[i] = cfg.y;
    }

    int nchunks = ceil((float)cfg.len / (float)cfg.chu);
    int start, stop;
    float* partialSum = malloc(nchunks * sizeof(float));

    for(int n = 0; n < nchunks; n++)
    {
        start = n * cfg.chu;
        stop = (start + cfg.chu < cfg.len) ? start + cfg.chu : cfg.len;

        partialSum[n] = 0.;

        for(int i = start; i < stop; i++)
        {
            r[i] = cfg.a * x[i] + y[i];
            partialSum[n] += r[i];
        }
    }

    float sum = 0.;

    for(int n = 0; n < nchunks; n++) sum += partialSum[n];


    hid_t file = H5Fcreate(fileout, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    writeFloatHDF5(sum, file, "/ChunkSum");
    writeFloatArrayHDF5(partialSum, nchunks, file, "/PartialSums");
    writeFloatArrayHDF5(r, cfg.len, file, "/ResultArray");

    H5Fclose(file);
    printf("Wrote %s\n", fileout);

    return 0;
}