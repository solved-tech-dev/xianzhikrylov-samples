/* Public C API: structure, numeric preparation, RHS and matrix updates. */
#include "sample_common.h"

static void make_rhs(xk_matrix *m, double scale)
{
    for (int i=0;i<4;++i) {
        m->b[i]=0;
        for (int j=m->row[i];j<m->row[i+1];++j) m->b[i]+=scale*m->a[j];
    }
}

int main(int argc, char **argv)
{
    const char *config=argc>2?argv[2]:"config.json";
    const int32_t rows[5]={0,2,5,8,10};
    const int32_t cols[10]={0,1,0,1,2,1,2,3,2,3};
    xk_matrix m={0};xk_binding b={0};xk_solution x={0};
    xianzhikrylov_handle_t h=NULL;
    xianzhikrylov_status_t st=XIANZHIKRYLOV_STATE_ERROR;
    int rc=1;
    m.h.rows=4;m.h.entries=10;m.h.rhs_count=1;m.h.scalar=1;
    m.row=malloc(sizeof(rows));m.col=malloc(sizeof(cols));
    m.a=malloc(10*sizeof(double));m.b=malloc(4*sizeof(double));
    if(!m.row||!m.col||!m.a||!m.b)goto done;
    memcpy(m.row,rows,sizeof(rows));memcpy(m.col,cols,sizeof(cols));
    for(int i=0;i<4;++i)for(int j=m.row[i];j<m.row[i+1];++j)m.a[j]=m.col[j]==i?4.0:-1.0;
    make_rhs(&m,1.0);
    if(!xk_binding_make(&b,&m,XIANZHIKRYLOV_CSR,1)||!xk_solution_init(&x,&m))goto done;
#define CHECK(call,expected) do { st=(call); if(st!=(expected)){fprintf(stderr,"%s returned %d\n",#call,(int)st);goto done;} } while(0)
#define PREPARE() CHECK(xianzhikrylov_execute(h,XIANZHIKRYLOV_PHASE_NUMERIC_PREPARE,b.matrix_object,NULL,NULL),XIANZHIKRYLOV_SUCCESS)
#define ANALYZE() CHECK(xianzhikrylov_execute(h,XIANZHIKRYLOV_PHASE_ANALYSIS,b.matrix_object,NULL,NULL),XIANZHIKRYLOV_SUCCESS)
#define SOLVE() do { CHECK(xianzhikrylov_execute(h,XIANZHIKRYLOV_PHASE_SOLVE,b.matrix_object,x.vector_object,b.rhs_object),XIANZHIKRYLOV_CONVERGED); CHECK(xianzhikrylov_print_info(h),XIANZHIKRYLOV_SUCCESS); if(!xk_verify(&m,&x,1e-6L))goto done; } while(0)
    CHECK(xianzhikrylov_create(0,&h),XIANZHIKRYLOV_SUCCESS);
    CHECK(xianzhikrylov_config_set(h,XIANZHIKRYLOV_CONFIG_FILE,config),XIANZHIKRYLOV_SUCCESS);
    puts("Initial structure and numeric preparation");ANALYZE();PREPARE();SOLVE();
    puts("RHS update: reuse structure and numeric preparation");make_rhs(&m,2.0);SOLVE();
    puts("Coefficient update: reuse structure, refresh numeric preparation");
    for(int i=0;i<4;++i)for(int j=m.row[i];j<m.row[i+1];++j)if(m.col[j]==i)m.a[j]+=1.0;
    make_rhs(&m,1.0);PREPARE();SOLVE();
    /* Change nnz and sparsity: recreate the descriptor, then analyze again. */
    puts("Structure update: recreate descriptor, analyze and prepare");
    xk_binding_free(&b);
    m.h.entries=4;
    for(int i=0;i<4;++i){m.row[i]=i;m.col[i]=i;m.a[i]=2.0+i;}m.row[4]=4;
    make_rhs(&m,3.0);
    if(!xk_binding_make(&b,&m,XIANZHIKRYLOV_CSR,1))goto done;
    ANALYZE();PREPARE();SOLVE();rc=0;
done:
    if(rc)fprintf(stderr,"staged sample failed, last status=%d\n",(int)st);
    if(h&&xianzhikrylov_destroy(h)!=XIANZHIKRYLOV_SUCCESS)rc=1;
    xk_solution_free(&x);xk_binding_free(&b);xk_matrix_free(&m);return rc;
}
