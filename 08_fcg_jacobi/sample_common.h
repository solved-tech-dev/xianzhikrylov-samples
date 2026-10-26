#ifndef XIANZHIKRYLOV_SAMPLE_COMMON_H
#define XIANZHIKRYLOV_SAMPLE_COMMON_H

#include "XianzhiKrylovAPI.h"
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char magic[8];
    uint32_t version, scalar;
    uint64_t rows, entries;
    uint32_t rhs_count, suggested_block;
    uint64_t reserved;
} xk_sample_header;
_Static_assert(sizeof(xk_sample_header) == 48, "XKSBIN1 header size");

typedef struct {
    char magic[8];
    uint32_t version, scalar;
    uint64_t rows, entries;
    uint32_t rhs_count, suggested_block, model_id, reserved32;
    uint64_t seed, reserved64;
} xk_pattern_header;
_Static_assert(sizeof(xk_pattern_header) == 64, "XKPAT1 header size");

typedef struct {
    xk_sample_header h;
    uint32_t model_id;
    uint64_t seed;
    int32_t *row, *col;
    double *a, *b;
    xianzhikrylov_complex *za, *zb;
} xk_matrix;

typedef struct {
    xianzhikrylov_system_data system;
    int32_t *row, *col, *lower_addr, *upper_addr;
    uint64_t *brow, *bcol, *bdiag;
    double *values, *offdiag, *lower_values, *upper_values;
} xk_binding;

typedef struct {
    xianzhikrylov_solution descriptor;
    double *x;
    xianzhikrylov_complex *z;
} xk_solution;

static void xk_matrix_free(xk_matrix *a)
{
    if (!a) return;
    free(a->row);
    free(a->col);
    free(a->a);
    free(a->b);
    free(a->za);
    free(a->zb);
    memset(a, 0, sizeof(*a));
}

static void xk_binding_free(xk_binding *b)
{
    if (!b) return;
    free(b->row);
    free(b->col);
    free(b->lower_addr);
    free(b->upper_addr);
    free(b->brow);
    free(b->bcol);
    free(b->bdiag);
    free(b->values);
    free(b->offdiag);
    free(b->lower_values);
    free(b->upper_values);
    memset(b, 0, sizeof(*b));
}

static void xk_solution_free(xk_solution *x)
{
    if (!x) return;
    free(x->x);
    free(x->z);
    memset(x, 0, sizeof(*x));
}

static int xk_size_ok(uint64_t count, size_t element_size)
{
    return count <= SIZE_MAX / element_size;
}

static int xk_read_array(FILE *f, void *data, uint64_t count,
                         size_t element_size)
{
    return xk_size_ok(count, element_size) &&
           (count == 0 ||
            fread(data, element_size, (size_t)count, f) == (size_t)count);
}

static uint64_t xk_mix64(uint64_t x)
{
    x += UINT64_C(0x9e3779b97f4a7c15);
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    return x ^ (x >> 31);
}

static double xk_unit(uint64_t seed, uint32_t model, uint64_t i,
                      uint64_t j, uint64_t salt)
{
    uint64_t v = seed ^ ((uint64_t)model << 56) ^ salt;
    v ^= (i + UINT64_C(0x9e3779b97f4a7c15)) *
         UINT64_C(0xd6e8feb86659fd93);
    v ^= (j + UINT64_C(0x517cc1b727220a95)) *
         UINT64_C(0xa5a3564e27f8862f);
    return (double)(xk_mix64(v) >> 11) * 0x1.0p-53;
}

static int xk_validate_csr(const xk_matrix *a)
{
    uint64_t i;
    int32_t k;
    if (!a->row || !a->col || a->row[0] != 0 ||
        a->row[a->h.rows] != (int32_t)a->h.entries)
        return 0;
    for (i = 0; i < a->h.rows; ++i) {
        if (a->row[i] < 0 || a->row[i] > a->row[i + 1])
            return 0;
        for (k = a->row[i]; k < a->row[i + 1]; ++k)
            if (a->col[k] < 0 || (uint64_t)a->col[k] >= a->h.rows ||
                (k > a->row[i] && a->col[k] <= a->col[k - 1]))
                return 0;
    }
    return 1;
}

static int xk_find_column(const xk_matrix *a, int32_t row, int32_t column)
{
    int32_t lo = a->row[row], hi = a->row[row + 1];
    while (lo < hi) {
        int32_t mid = lo + (hi - lo) / 2;
        if (a->col[mid] < column)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo < a->row[row + 1] && a->col[lo] == column ? lo : -1;
}

static int xk_generate_real_values(xk_matrix *a)
{
    uint64_t i;
    uint32_t rhs;
    if (!xk_size_ok(a->h.entries, sizeof(double)) ||
        !xk_size_ok(a->h.rows * a->h.rhs_count, sizeof(double)))
        return 0;
    a->a = (double *)calloc((size_t)a->h.entries, sizeof(double));
    a->b = (double *)calloc((size_t)(a->h.rows * a->h.rhs_count),
                            sizeof(double));
    if (!a->a || !a->b) return 0;
    for (i = 0; i < a->h.rows; ++i) {
        int32_t diag = -1;
        double magnitude_sum = 0.0;
        int32_t k;
        for (k = a->row[i]; k < a->row[i + 1]; ++k) {
            double value;
            if (a->col[k] == (int32_t)i) {
                diag = k;
                continue;
            }
            if (a->model_id == 2) {
                value = -(0.20 + 1.80 * xk_unit(a->seed, a->model_id,
                    i, (uint64_t)a->col[k], UINT64_C(0x32)));
            } else {
                uint64_t lo = i < (uint64_t)a->col[k] ?
                    i : (uint64_t)a->col[k];
                uint64_t hi = i > (uint64_t)a->col[k] ?
                    i : (uint64_t)a->col[k];
                value = -(0.20 + 1.80 * xk_unit(a->seed, a->model_id,
                    lo, hi, UINT64_C(0x31)));
            }
            a->a[k] = value;
            magnitude_sum += fabs(value);
        }
        if (diag < 0) return 0;
        a->a[diag] = magnitude_sum + 1.0 +
            0.25 * xk_unit(a->seed, a->model_id, i, i, UINT64_C(0x33));
    }
    for (rhs = 0; rhs < a->h.rhs_count; ++rhs) {
        for (i = 0; i < a->h.rows; ++i) {
            long double sum = 0.0L;
            int32_t k;
            for (k = a->row[i]; k < a->row[i + 1]; ++k)
                sum += a->a[k];
            a->b[(uint64_t)rhs * a->h.rows + i] = (double)sum;
        }
    }
    return 1;
}

static int xk_generate_complex_values(xk_matrix *a)
{
    uint64_t i;
    uint32_t rhs;
    static const double xr[3] = {1.0, 1.0, 1.0};
    static const double xi[3] = {0.0, 0.25, -0.25};
    if (!xk_size_ok(a->h.entries, sizeof(xianzhikrylov_complex)) ||
        !xk_size_ok(a->h.rows * a->h.rhs_count,
                    sizeof(xianzhikrylov_complex)))
        return 0;
    a->za = (xianzhikrylov_complex *)calloc(
        (size_t)a->h.entries, sizeof(xianzhikrylov_complex));
    a->zb = (xianzhikrylov_complex *)calloc(
        (size_t)(a->h.rows * a->h.rhs_count),
        sizeof(xianzhikrylov_complex));
    if (!a->za || !a->zb) return 0;
    for (i = 0; i < a->h.rows; ++i) {
        int32_t diag = -1;
        long double magnitude_sum = 0.0L;
        int32_t k;
        for (k = a->row[i]; k < a->row[i + 1]; ++k) {
            long double re, im;
            if (a->col[k] == (int32_t)i) {
                diag = k;
                continue;
            }
            re = -(0.05L + 0.45L * xk_unit(a->seed, a->model_id,
                i, (uint64_t)a->col[k], UINT64_C(0x44)));
            im = (xk_unit(a->seed, a->model_id, i,
                (uint64_t)a->col[k], UINT64_C(0x45)) - 0.5L) * 0.60L;
            a->za[k].real = (double)re;
            a->za[k].imag = (double)im;
            magnitude_sum += hypotl(re, im);
        }
        if (diag < 0) return 0;
        a->za[diag].real = (double)(magnitude_sum + 1.0L +
            0.20L * xk_unit(a->seed, a->model_id, i, i,
                            UINT64_C(0x46)));
        a->za[diag].imag = 0.0;
    }
    for (rhs = 0; rhs < a->h.rhs_count; ++rhs) {
        uint32_t which = rhs < 3 ? rhs : 2;
        for (i = 0; i < a->h.rows; ++i) {
            long double re = 0.0L, im = 0.0L;
            int32_t k;
            for (k = a->row[i]; k < a->row[i + 1]; ++k) {
                re += a->za[k].real * xr[which] -
                      a->za[k].imag * xi[which];
                im += a->za[k].real * xi[which] +
                      a->za[k].imag * xr[which];
            }
            a->zb[(uint64_t)rhs * a->h.rows + i].real = (double)re;
            a->zb[(uint64_t)rhs * a->h.rows + i].imag = (double)im;
        }
    }
    return 1;
}

static int xk_load_pattern(xk_matrix *a, FILE *f, const char *path)
{
    xk_pattern_header p;
    long bytes;
    if (fread(&p, sizeof(p), 1, f) != 1 ||
        memcmp(p.magic, "XKPAT1\0\0", 8) || p.version != 1 ||
        (p.scalar != 1 && p.scalar != 2) || !p.rows || !p.entries ||
        p.rows > INT32_MAX || p.entries > INT32_MAX ||
        p.rhs_count < 1 || p.rhs_count > 10 ||
        p.model_id < 1 || p.model_id > 4 ||
        p.reserved32 || p.reserved64 || !p.seed) {
        fprintf(stderr, "Cannot read sample pattern: %s\n", path);
        return 0;
    }
    if (fseek(f, 0, SEEK_END) != 0 ||
        (bytes = ftell(f)) < 0 ||
        (uint64_t)bytes != sizeof(p) + 4 * (p.rows + 1) + 4 * p.entries) {
        fprintf(stderr, "Truncated or unexpected pattern file\n");
        return 0;
    }
    if (fseek(f, (long)sizeof(p), SEEK_SET) != 0) return 0;
    if (!xk_size_ok(p.rows + 1, sizeof(int32_t)) ||
        !xk_size_ok(p.entries, sizeof(int32_t)))
        return 0;
    a->row = (int32_t *)malloc((size_t)(p.rows + 1) * sizeof(int32_t));
    a->col = (int32_t *)malloc((size_t)p.entries * sizeof(int32_t));
    if (!a->row || !a->col ||
        !xk_read_array(f, a->row, p.rows + 1, sizeof(int32_t)) ||
        !xk_read_array(f, a->col, p.entries, sizeof(int32_t)))
        return 0;
    memcpy(a->h.magic, p.magic, 8);
    a->h.version = p.version;
    a->h.scalar = p.scalar;
    a->h.rows = p.rows;
    a->h.entries = p.entries;
    a->h.rhs_count = p.rhs_count;
    a->h.suggested_block = p.suggested_block;
    a->model_id = p.model_id;
    a->seed = p.seed;
    if (!xk_validate_csr(a) ||
        !(p.scalar == 1 ? xk_generate_real_values(a) :
                          xk_generate_complex_values(a)))
        return 0;
    printf("Matrix: rows=%llu nnz=%llu RHS=%u scalar=%s "
           "pattern_model=%u seed=%016llx\n",
           (unsigned long long)a->h.rows,
           (unsigned long long)a->h.entries, a->h.rhs_count,
           a->h.scalar == 1 ? "FP64" : "complex FP64",
           a->model_id, (unsigned long long)a->seed);
    return 1;
}

static int xk_load_values(xk_matrix *a, FILE *f, const char *path)
{
    long bytes;
    uint64_t product;
    if (fread(&a->h, sizeof(a->h), 1, f) != 1 ||
        memcmp(a->h.magic, "XKSBIN1\0", 8) || a->h.version != 1 ||
        (a->h.scalar != 1 && a->h.scalar != 2) || a->h.reserved ||
        !a->h.rows || a->h.rows > INT32_MAX ||
        a->h.entries > INT32_MAX ||
        a->h.rhs_count < 1 || a->h.rhs_count > 10) {
        fprintf(stderr, "Cannot read sample matrix: %s\n", path);
        return 0;
    }
    product = a->h.entries + a->h.rows * a->h.rhs_count;
    if (fseek(f, 0, SEEK_END) != 0 || (bytes = ftell(f)) < 0 ||
        (uint64_t)bytes != 48 + 4 * (a->h.rows + 1) +
            4 * a->h.entries +
            (a->h.scalar == 1 ? 8ULL : 16ULL) * product ||
        fseek(f, 48, SEEK_SET) != 0)
        return 0;
    a->row = (int32_t *)malloc((size_t)(a->h.rows + 1) * sizeof(int32_t));
    a->col = (int32_t *)malloc((size_t)a->h.entries * sizeof(int32_t));
    if (!a->row || !a->col ||
        !xk_read_array(f, a->row, a->h.rows + 1, sizeof(int32_t)) ||
        !xk_read_array(f, a->col, a->h.entries, sizeof(int32_t)))
        return 0;
    if (a->h.scalar == 1) {
        a->a = (double *)malloc((size_t)a->h.entries * sizeof(double));
        a->b = (double *)malloc(
            (size_t)(a->h.rows * a->h.rhs_count) * sizeof(double));
        if (!a->a || !a->b ||
            !xk_read_array(f, a->a, a->h.entries, sizeof(double)) ||
            !xk_read_array(f, a->b, a->h.rows * a->h.rhs_count,
                           sizeof(double)))
            return 0;
    } else {
        a->za = (xianzhikrylov_complex *)malloc(
            (size_t)a->h.entries * sizeof(xianzhikrylov_complex));
        a->zb = (xianzhikrylov_complex *)malloc(
            (size_t)(a->h.rows * a->h.rhs_count) *
            sizeof(xianzhikrylov_complex));
        if (!a->za || !a->zb ||
            !xk_read_array(f, a->za, a->h.entries,
                           sizeof(xianzhikrylov_complex)) ||
            !xk_read_array(f, a->zb, a->h.rows * a->h.rhs_count,
                           sizeof(xianzhikrylov_complex)))
            return 0;
    }
    if (!xk_validate_csr(a)) return 0;
    printf("Matrix: rows=%llu nnz=%llu RHS=%u scalar=%s\n",
           (unsigned long long)a->h.rows,
           (unsigned long long)a->h.entries, a->h.rhs_count,
           a->h.scalar == 1 ? "FP64" : "complex FP64");
    return 1;
}

static int xk_matrix_load(xk_matrix *a, const char *path)
{
    FILE *f;
    char magic[8];
    int ok;
    if (!a || !path) return 0;
    memset(a, 0, sizeof(*a));
    f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Cannot open %s: %s\n", path, strerror(errno));
        return 0;
    }
    if (fread(magic, sizeof(magic), 1, f) != 1) {
        fclose(f);
        return 0;
    }
    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return 0;
    }
    if (!memcmp(magic, "XKPAT1\0\0", 8))
        ok = xk_load_pattern(a, f, path);
    else
        ok = xk_load_values(a, f, path);
    fclose(f);
    if (!ok) xk_matrix_free(a);
    return ok;
}

static xianzhikrylov_input_buffer xk_input(const void *data, size_t bytes)
{
    xianzhikrylov_input_buffer result;
    result.data = data;
    result.bytes = (uint64_t)bytes;
    return result;
}

static void xk_init_matrix_descriptor(xianzhikrylov_matrix *m,
                                       const xk_matrix *a)
{
    memset(m, 0, sizeof(*m));
    m->abi_version = XIANZHIKRYLOV_DATA_ABI;
    m->struct_size = sizeof(*m);
    m->scalar_type = a->h.scalar;
    m->index_type = XIANZHIKRYLOV_INDEX_I32;
    m->block_size = 1;
    m->block_layout = XIANZHIKRYLOV_BLOCK_ROW_MAJOR;
    m->memory_space = XIANZHIKRYLOV_HOST;
    m->device_id = -1;
    m->rows = a->h.rows;
    m->columns = a->h.rows;
    m->entries = a->h.entries;
}

static void xk_init_vector_descriptor(xianzhikrylov_vector *v, uint32_t scalar,
                                      uint64_t rows, uint64_t rhs,
                                      xianzhikrylov_input_buffer data)
{
    memset(v, 0, sizeof(*v));
    v->abi_version = XIANZHIKRYLOV_DATA_ABI;
    v->struct_size = sizeof(*v);
    v->scalar_type = scalar;
    v->layout = XIANZHIKRYLOV_RHS_MAJOR;
    v->memory_space = XIANZHIKRYLOV_HOST;
    v->device_id = -1;
    v->rows = rows;
    v->rhs_count = rhs;
    v->leading_dimension = rows;
    v->data = data;
}

typedef struct {
    uint64_t column;
    double value[16];
} xk_bsr_entry;

static int xk_compare_bsr(const void *left, const void *right)
{
    const xk_bsr_entry *a = (const xk_bsr_entry *)left;
    const xk_bsr_entry *b = (const xk_bsr_entry *)right;
    return a->column < b->column ? -1 : a->column > b->column;
}

static int xk_convert_bsr4(xk_binding *b, const xk_matrix *a)
{
    uint64_t block_rows = a->h.rows / 4;
    size_t bcol_count = 0, bcol_cap = 4096;
    size_t max_entries = 0;
    xk_bsr_entry *tmp;
    uint64_t br;
    if (a->h.scalar != 1 || a->h.rows % 4) return 0;
    b->brow = (uint64_t *)calloc((size_t)block_rows + 1, sizeof(uint64_t));
    b->bdiag = (uint64_t *)calloc((size_t)block_rows, sizeof(uint64_t));
    b->bcol = (uint64_t *)malloc(bcol_cap * sizeof(uint64_t));
    b->values = (double *)malloc(bcol_cap * 16 * sizeof(double));
    if (!b->brow || !b->bdiag || !b->bcol || !b->values) return 0;
    for (br = 0; br < block_rows; ++br) {
        size_t block_entries = 0;
        uint64_t r;
        for (r = 0; r < 4; ++r) {
            size_t row_entries =
                (size_t)(a->row[br * 4 + r + 1] - a->row[br * 4 + r]);
            block_entries += row_entries;
        }
        if (block_entries > max_entries) max_entries = block_entries;
    }
    tmp = (xk_bsr_entry *)malloc((max_entries ? max_entries : 1) *
                                 sizeof(xk_bsr_entry));
    if (!tmp) return 0;
    for (br = 0; br < block_rows; ++br) {
        size_t count = 0;
        uint64_t r;
        int32_t diag_block = -1;
        for (r = 0; r < 4; ++r) {
            int32_t k;
            for (k = a->row[br * 4 + r]; k < a->row[br * 4 + r + 1]; ++k) {
                uint64_t block_column = (uint64_t)a->col[k] / 4;
                size_t p;
                for (p = 0; p < count; ++p)
                    if (tmp[p].column == block_column) break;
                if (p == count) {
                    memset(tmp[p].value, 0, sizeof(tmp[p].value));
                    tmp[p].column = block_column;
                    ++count;
                }
                tmp[p].value[r * 4 + (uint64_t)a->col[k] % 4] = a->a[k];
            }
        }
        qsort(tmp, count, sizeof(*tmp), xk_compare_bsr);
        if (bcol_count + count > bcol_cap) {
            bcol_cap *= 2;
            while (bcol_cap < bcol_count + count) bcol_cap *= 2;
            {
                uint64_t *nc = (uint64_t *)realloc(
                    b->bcol, bcol_cap * sizeof(uint64_t));
                double *nv;
                if (!nc) {
                    free(tmp);
                    return 0;
                }
                b->bcol = nc;
                nv = (double *)realloc(
                    b->values, bcol_cap * 16 * sizeof(double));
                if (!nv) {
                    free(tmp);
                    return 0;
                }
                b->values = nv;
            }
        }
        {
            size_t p;
            for (p = 0; p < count; ++p) {
                if (tmp[p].column == br) diag_block = (int32_t)bcol_count;
                b->bcol[bcol_count] = tmp[p].column;
                memcpy(&b->values[bcol_count * 16], tmp[p].value,
                       16 * sizeof(double));
                ++bcol_count;
            }
        }
        if (diag_block < 0) {
            free(tmp);
            return 0;
        }
        b->bdiag[br] = (uint64_t)diag_block;
        b->brow[br + 1] = bcol_count;
    }
    free(tmp);
    xk_init_matrix_descriptor(&b->system.matrix, a);
    b->system.matrix.format = XIANZHIKRYLOV_BSR;
    b->system.matrix.index_type = XIANZHIKRYLOV_INDEX_I64;
    b->system.matrix.block_size = 4;
    b->system.matrix.rows = block_rows;
    b->system.matrix.columns = block_rows;
    b->system.matrix.entries = bcol_count;
    b->system.matrix.row_offsets = xk_input(
        b->brow, (size_t)(block_rows + 1) * sizeof(uint64_t));
    b->system.matrix.column_indices = xk_input(
        b->bcol, bcol_count * sizeof(uint64_t));
    b->system.matrix.diagonal_positions = xk_input(
        b->bdiag, (size_t)block_rows * sizeof(uint64_t));
    b->system.matrix.values = xk_input(
        b->values, bcol_count * 16 * sizeof(double));
    return 1;
}

static int xk_convert_msr(xk_binding *b, const xk_matrix *a)
{
    uint64_t i;
    size_t off_count = 0;
    if (a->h.scalar != 1 || a->h.entries < a->h.rows) return 0;
    b->row = (int32_t *)calloc((size_t)a->h.rows + 1, sizeof(int32_t));
    b->values = (double *)calloc((size_t)a->h.rows, sizeof(double));
    b->col = (int32_t *)malloc((size_t)a->h.entries * sizeof(int32_t));
    b->offdiag = (double *)malloc((size_t)a->h.entries * sizeof(double));
    if (!b->row || !b->values || !b->col || !b->offdiag) return 0;
    for (i = 0; i < a->h.rows; ++i) {
        int32_t diag = -1, k;
        for (k = a->row[i]; k < a->row[i + 1]; ++k) {
            if (a->col[k] == (int32_t)i) {
                diag = k;
            } else {
                b->col[off_count] = a->col[k];
                b->offdiag[off_count] = a->a[k];
                ++off_count;
            }
        }
        if (diag < 0) return 0;
        b->values[i] = a->a[diag];
        b->row[i + 1] = (int32_t)off_count;
    }
    xk_init_matrix_descriptor(&b->system.matrix, a);
    b->system.matrix.format = XIANZHIKRYLOV_MSR;
    b->system.matrix.entries = off_count;
    b->system.matrix.row_offsets = xk_input(
        b->row, (size_t)(a->h.rows + 1) * sizeof(int32_t));
    b->system.matrix.column_indices = xk_input(
        b->col, off_count * sizeof(int32_t));
    b->system.matrix.values = xk_input(
        b->values, (size_t)a->h.rows * sizeof(double));
    b->system.matrix.offdiagonal_values = xk_input(
        b->offdiag, off_count * sizeof(double));
    return 1;
}

static int xk_convert_ldu(xk_binding *b, const xk_matrix *a)
{
    uint64_t i;
    size_t face_count = 0;
    if (a->h.scalar != 1) return 0;
    b->values = (double *)calloc((size_t)a->h.rows, sizeof(double));
    b->lower_addr = (int32_t *)malloc((size_t)a->h.entries * sizeof(int32_t));
    b->upper_addr = (int32_t *)malloc((size_t)a->h.entries * sizeof(int32_t));
    b->lower_values = (double *)malloc((size_t)a->h.entries * sizeof(double));
    b->upper_values = (double *)malloc((size_t)a->h.entries * sizeof(double));
    if (!b->values || !b->lower_addr || !b->upper_addr ||
        !b->lower_values || !b->upper_values)
        return 0;
    for (i = 0; i < a->h.rows; ++i) {
        int32_t diag = -1, k;
        for (k = a->row[i]; k < a->row[i + 1]; ++k) {
            int32_t j = a->col[k], mirror;
            if (j == (int32_t)i) {
                diag = k;
                continue;
            }
            if (j < (int32_t)i) continue;
            mirror = xk_find_column(a, j, (int32_t)i);
            if (mirror < 0) return 0;
            b->lower_addr[face_count] = (int32_t)i;
            b->upper_addr[face_count] = j;
            b->lower_values[face_count] = a->a[mirror];
            b->upper_values[face_count] = a->a[k];
            ++face_count;
        }
        if (diag < 0) return 0;
        b->values[i] = a->a[diag];
    }
    xk_init_matrix_descriptor(&b->system.matrix, a);
    b->system.matrix.format = XIANZHIKRYLOV_LDU;
    b->system.matrix.entries = face_count;
    b->system.matrix.values = xk_input(
        b->values, (size_t)a->h.rows * sizeof(double));
    b->system.matrix.lower_address = xk_input(
        b->lower_addr, face_count * sizeof(int32_t));
    b->system.matrix.upper_address = xk_input(
        b->upper_addr, face_count * sizeof(int32_t));
    b->system.matrix.lower_values = xk_input(
        b->lower_values, face_count * sizeof(double));
    b->system.matrix.upper_values = xk_input(
        b->upper_values, face_count * sizeof(double));
    return 1;
}

static int xk_binding_make(xk_binding *b, const xk_matrix *a,
                           uint32_t format, uint32_t block)
{
    xianzhikrylov_input_buffer rhs_data;
    if (!b || !a) return 0;
    memset(b, 0, sizeof(*b));
    if (format == XIANZHIKRYLOV_CSR) {
        xk_init_matrix_descriptor(&b->system.matrix, a);
        b->system.matrix.format = XIANZHIKRYLOV_CSR;
        b->system.matrix.row_offsets = xk_input(
            a->row, (size_t)(a->h.rows + 1) * sizeof(int32_t));
        b->system.matrix.column_indices = xk_input(
            a->col, (size_t)a->h.entries * sizeof(int32_t));
        b->system.matrix.values = a->h.scalar == 1 ?
            xk_input(a->a, (size_t)a->h.entries * sizeof(double)) :
            xk_input(a->za, (size_t)a->h.entries *
                             sizeof(xianzhikrylov_complex));
    } else if (format == XIANZHIKRYLOV_BSR && block == 4) {
        if (!xk_convert_bsr4(b, a)) return 0;
    } else if (format == XIANZHIKRYLOV_MSR && block == 1) {
        if (!xk_convert_msr(b, a)) return 0;
    } else if (format == XIANZHIKRYLOV_LDU && block == 1) {
        if (!xk_convert_ldu(b, a)) return 0;
    } else {
        fprintf(stderr, "Unsupported sample conversion\n");
        return 0;
    }
    rhs_data = a->h.scalar == 1 ?
        xk_input(a->b, (size_t)(a->h.rows * a->h.rhs_count) *
                       sizeof(double)) :
        xk_input(a->zb, (size_t)(a->h.rows * a->h.rhs_count) *
                        sizeof(xianzhikrylov_complex));
    b->system.abi_version = XIANZHIKRYLOV_DATA_ABI;
    b->system.struct_size = sizeof(b->system);
    xk_init_vector_descriptor(&b->system.rhs, a->h.scalar,
                              a->h.rows, a->h.rhs_count, rhs_data);
    return 1;
}

static int xk_solution_init(xk_solution *x, const xk_matrix *a)
{
    uint64_t count;
    if (!x || !a) return 0;
    memset(x, 0, sizeof(*x));
    if (!xk_size_ok(a->h.rows * a->h.rhs_count,
                    a->h.scalar == 1 ? sizeof(double) :
                                       sizeof(xianzhikrylov_complex)))
        return 0;
    count = a->h.rows * a->h.rhs_count;
    if (a->h.scalar == 1) {
        x->x = (double *)calloc((size_t)count, sizeof(double));
        if (!x->x) return 0;
        x->descriptor.data.data = x->x;
        x->descriptor.data.bytes = count * sizeof(double);
    } else {
        x->z = (xianzhikrylov_complex *)calloc(
            (size_t)count, sizeof(xianzhikrylov_complex));
        if (!x->z) return 0;
        x->descriptor.data.data = x->z;
        x->descriptor.data.bytes =
            count * sizeof(xianzhikrylov_complex);
    }
    x->descriptor.abi_version = XIANZHIKRYLOV_DATA_ABI;
    x->descriptor.struct_size = sizeof(x->descriptor);
    x->descriptor.scalar_type = a->h.scalar;
    x->descriptor.layout = XIANZHIKRYLOV_RHS_MAJOR;
    x->descriptor.memory_space = XIANZHIKRYLOV_HOST;
    x->descriptor.device_id = -1;
    x->descriptor.rows = a->h.rows;
    x->descriptor.rhs_count = a->h.rhs_count;
    x->descriptor.leading_dimension = a->h.rows;
    return 1;
}

static int xk_verify(const xk_matrix *a, const xk_solution *x,
                     long double tolerance)
{
    uint32_t rhs;
    int all = 1;
    for (rhs = 0; rhs < a->h.rhs_count; ++rhs) {
        long double rr = 0.0L, bb = 0.0L, maximum = 0.0L;
        int finite = 1;
        uint64_t i;
        for (i = 0; i < a->h.rows; ++i) {
            uint64_t q = (uint64_t)rhs * a->h.rows + i;
            long double residual = 0.0L, rhs_norm = 0.0L;
            int32_t k;
            if (a->h.scalar == 1) {
                residual = a->b[q];
                rhs_norm = fabsl(residual);
                for (k = a->row[i]; k < a->row[i + 1]; ++k) {
                    uint64_t p = (uint64_t)rhs * a->h.rows + a->col[k];
                    residual -= (long double)a->a[k] * x->x[p];
                }
            } else {
                long double residual_re = a->zb[q].real;
                long double residual_im = a->zb[q].imag;
                rhs_norm = hypotl(residual_re, residual_im);
                for (k = a->row[i]; k < a->row[i + 1]; ++k) {
                    uint64_t p = (uint64_t)rhs * a->h.rows + a->col[k];
                    residual_re -= a->za[k].real * x->z[p].real -
                                   a->za[k].imag * x->z[p].imag;
                    residual_im -= a->za[k].real * x->z[p].imag +
                                   a->za[k].imag * x->z[p].real;
                }
                residual = hypotl(residual_re, residual_im);
            }
            if (!isfinite(residual) || !isfinite(rhs_norm)) finite = 0;
            rr += residual * residual;
            bb += rhs_norm * rhs_norm;
            if (residual > maximum) maximum = residual;
        }
        {
            long double relative = bb > 0.0L ? sqrtl(rr / bb) : sqrtl(rr);
            int pass = finite &&
                (a->h.scalar == 2 ? maximum < tolerance :
                                    relative <= tolerance);
            printf("Independent residual RHS %u: "
                   "relative_l2=%.12Le absmax=%.12Le %s\n",
                   rhs + 1, relative, maximum, pass ? "PASS" : "FAIL");
            all = all && pass;
        }
    }
    return all;
}

#endif
