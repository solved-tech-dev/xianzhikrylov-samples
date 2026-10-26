#pragma once
#include "XianzhiKrylovAPI.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <vector>

namespace sample {
// Sample-file format only: the library receives arrays, not this file header.
struct Header {
    char magic[8];
    uint32_t version, scalar;
    uint64_t rows, entries;
    uint32_t rhs_count, suggested_block;
    uint64_t reserved;
};
static_assert(sizeof(Header)==48);
struct PatternHeader {
    char magic[8];
    uint32_t version, scalar;
    uint64_t rows, entries;
    uint32_t rhs_count, suggested_block, model_id, reserved32;
    uint64_t seed, reserved64;
};
static_assert(sizeof(PatternHeader)==64);
template<class T> xianzhikrylov_input_buffer input(const std::vector<T>& v) {
    return {v.data(),uint64_t(v.size()*sizeof(T))};
}
template<class T> bool read_array(std::ifstream& in,std::vector<T>& v,size_t n) {
    v.resize(n);return bool(in.read(reinterpret_cast<char*>(v.data()),n*sizeof(T)));
}
struct Matrix {
    Header h{};
    uint32_t model_id=0;
    uint64_t seed=0;
    std::vector<int32_t> row,col;
    std::vector<double> a,b;
    std::vector<xianzhikrylov_complex> za,zb;
    bool load(const std::filesystem::path& file) {
        std::ifstream in(file,std::ios::binary);
        char magic[8]{};
        if(!in.read(magic,sizeof(magic))) {
            std::fprintf(stderr,"Cannot read sample matrix: %s\n",file.c_str());return false;
        }
        if(!std::memcmp(magic,"XKPAT1\0\0",8)) return load_pattern(file);
        if(std::memcmp(magic,"XKSBIN1\0",8)) {
            std::fprintf(stderr,"Cannot read sample matrix: %s\n",file.c_str());return false;
        }
        in.seekg(0);
        if(!in.read(reinterpret_cast<char*>(&h),sizeof(h)) ||
           h.version!=1 || (h.scalar!=1&&h.scalar!=2) || h.reserved || !h.rows ||
           h.rows>INT32_MAX || h.entries>INT32_MAX || h.rhs_count<1 || h.rhs_count>10) {
            std::fprintf(stderr,"Cannot read sample matrix: %s\n",file.c_str());return false;
        }
        const uint64_t bytes=48+4*(h.rows+1)+4*h.entries+(h.scalar==1?8:16)*(h.entries+h.rows*h.rhs_count);
        in.seekg(0,std::ios::end);
        if(uint64_t(in.tellg())!=bytes){std::fprintf(stderr,"Truncated or unexpected matrix file\n");return false;}
        in.seekg(48);
        bool ok=read_array(in,row,h.rows+1)&&read_array(in,col,h.entries);
        if(h.scalar==1)ok=ok&&read_array(in,a,h.entries)&&read_array(in,b,h.rows*h.rhs_count);
        else ok=ok&&read_array(in,za,h.entries)&&read_array(in,zb,h.rows*h.rhs_count);
        if(!ok||row[0]!=0||row.back()!=int32_t(h.entries))return false;
        if(h.scalar==1){
            for(double v:a)if(!std::isfinite(v))return false;
            for(double v:b)if(!std::isfinite(v))return false;
        }else{
            for(auto v:za)if(!std::isfinite(v.real)||!std::isfinite(v.imag))return false;
            for(auto v:zb)if(!std::isfinite(v.real)||!std::isfinite(v.imag))return false;
        }
        for(size_t i=0;i<h.rows;++i){
            if(row[i]<0||row[i]>row[i+1]||row[i+1]>int32_t(h.entries))return false;
            for(int k=row[i];k<row[i+1];++k)
                if(col[k]<0||col[k]>=int32_t(h.rows)||(k>row[i]&&col[k]<=col[k-1]))return false;
        }
        std::printf("Matrix: rows=%llu nnz=%llu RHS=%u scalar=%s\n",
            (unsigned long long)h.rows,(unsigned long long)h.entries,h.rhs_count,h.scalar==1?"FP64":"complex FP64");
        return true;
    }

    static uint64_t mix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    static double unit(uint64_t seed, uint32_t model, uint64_t i,
                       uint64_t j, uint64_t salt) {
        uint64_t v = seed ^ (uint64_t(model) << 56) ^ salt;
        v ^= (i + 0x9e3779b97f4a7c15ULL) * 0xd6e8feb86659fd93ULL;
        v ^= (j + 0x517cc1b727220a95ULL) * 0xa5a3564e27f8862fULL;
        return double(mix64(v) >> 11) * 0x1.0p-53;
    }

    bool load_pattern(const std::filesystem::path& file) {
        std::ifstream in(file,std::ios::binary);
        PatternHeader p{};
        if(!in.read(reinterpret_cast<char*>(&p),sizeof(p)) ||
           std::memcmp(p.magic,"XKPAT1\0\0",8) || p.version!=1 ||
           (p.scalar!=1&&p.scalar!=2) || !p.rows || !p.entries ||
           p.rows>INT32_MAX || p.entries>INT32_MAX || p.rhs_count<1 ||
           p.rhs_count>10 || p.model_id<1 || p.model_id>4 ||
           !p.seed || p.reserved32 || p.reserved64) {
            std::fprintf(stderr,"Cannot read sample pattern: %s\n",file.c_str());return false;
        }
        const uint64_t bytes=sizeof(p)+4*(p.rows+1)+4*p.entries;
        in.seekg(0,std::ios::end);
        if(uint64_t(in.tellg())!=bytes) {
            std::fprintf(stderr,"Truncated or unexpected pattern file\n");return false;
        }
        in.seekg(sizeof(p));
        bool ok=read_array(in,row,p.rows+1)&&read_array(in,col,p.entries);
        if(!ok||row[0]!=0||row.back()!=int32_t(p.entries))return false;
        for(size_t i=0;i<p.rows;++i) {
            if(row[i]<0||row[i]>row[i+1]||row[i+1]>int32_t(p.entries))
                return false;
            for(int k=row[i];k<row[i+1];++k)
                if(col[k]<0||col[k]>=int32_t(p.rows)||
                   (k>row[i]&&col[k]<=col[k-1]))return false;
        }
        std::memcpy(h.magic,p.magic,8);
        h.version=p.version;h.scalar=p.scalar;h.rows=p.rows;h.entries=p.entries;
        h.rhs_count=p.rhs_count;h.suggested_block=p.suggested_block;h.reserved=0;
        model_id=p.model_id;seed=p.seed;
        if(!generate_values())return false;
        std::printf("Matrix: rows=%llu nnz=%llu RHS=%u scalar=%s "
                    "pattern_model=%u seed=%016llx\n",
            (unsigned long long)h.rows,(unsigned long long)h.entries,
            h.rhs_count,h.scalar==1?"FP64":"complex FP64",model_id,
            (unsigned long long)seed);
        return true;
    }

    bool generate_values() {
        const auto symmetric_value=[&](int i,int j)->double {
            const uint64_t lo=uint64_t(std::min(i,j)),hi=uint64_t(std::max(i,j));
            return -(0.20+1.80*unit(seed,model_id,lo,hi,0x31));
        };
        if(h.scalar==1) {
            a.assign(h.entries,0.0);
            for(int64_t i=0;i<int64_t(h.rows);++i) {
                int diag=-1;
                double magnitude_sum=0.0;
                for(int k=row[i];k<row[i+1];++k) {
                    if(col[k]==i) {diag=k;continue;}
                    double value;
                    if(model_id==2)
                        value=-(0.20+1.80*
                            unit(seed,model_id,uint64_t(i),uint64_t(col[k]),0x32));
                    else
                        value=symmetric_value(i,col[k]);
                    a[k]=value;magnitude_sum+=std::abs(value);
                }
                if(diag<0)return false;
                a[diag]=magnitude_sum+1.0+
                    0.25*unit(seed,model_id,uint64_t(i),uint64_t(i),0x33);
            }
            b.assign(size_t(h.rows)*h.rhs_count,0.0);
            for(uint32_t rhs=0;rhs<h.rhs_count;++rhs)
                for(int64_t i=0;i<int64_t(h.rows);++i) {
                    long double sum=0.0L;
                    for(int k=row[i];k<row[i+1];++k)
                        sum+=a[k];
                    b[size_t(rhs)*h.rows+i]=double(sum);
                }
            return true;
        }
        za.assign(h.entries,{});
        for(int64_t i=0;i<int64_t(h.rows);++i) {
            int diag=-1;
            long double magnitude_sum=0.0L;
            for(int k=row[i];k<row[i+1];++k) {
                if(col[k]==i) {diag=k;continue;}
                const double s=unit(seed,model_id,uint64_t(i),uint64_t(col[k]),0x44);
                const long double re=-(0.05+0.45*s);
                const long double im=(unit(seed,model_id,uint64_t(i),
                                         uint64_t(col[k]),0x45)-0.5)*0.60;
                za[k]={double(re),double(im)};
                magnitude_sum+=std::hypot(re,im);
            }
            if(diag<0)return false;
            za[diag]={double(magnitude_sum+1.0+
                            0.20*unit(seed,model_id,uint64_t(i),
                                     uint64_t(i),0x46)),0.0};
        }
        const double xr[3]={1.0,1.0,1.0};
        const double xi[3]={0.0,0.25,-0.25};
        zb.assign(size_t(h.rows)*h.rhs_count,{});
        for(uint32_t rhs=0;rhs<h.rhs_count;++rhs) {
            const int which=std::min<int>(rhs,2);
            for(int64_t i=0;i<int64_t(h.rows);++i) {
                long double re=0.0L,im=0.0L;
                for(int k=row[i];k<row[i+1];++k) {
                    re+=za[k].real*xr[which]-za[k].imag*xi[which];
                    im+=za[k].real*xi[which]+za[k].imag*xr[which];
                }
                zb[size_t(rhs)*h.rows+i]={double(re),double(im)};
            }
        }
        return true;
    }
};

// CPU-side storage conversion is explicit and occurs before library preparation.
// Keep the original CSR matrix for independent residual verification.
struct Binding {
    xianzhikrylov_system_data system{};
    std::vector<int32_t> row,col,lo,hi;
    std::vector<uint64_t> brow,bcol,bdiag;
    std::vector<double> values,off,lower,upper;
    bool make(const Matrix& a,uint32_t format,uint32_t block=1) {
        system.abi_version=XIANZHIKRYLOV_DATA_ABI;system.struct_size=sizeof(system);
        auto& m=system.matrix;m.abi_version=XIANZHIKRYLOV_DATA_ABI;m.struct_size=sizeof(m);
        m.format=format;m.scalar_type=a.h.scalar;m.index_type=XIANZHIKRYLOV_INDEX_I32;
        m.block_size=block;m.block_layout=XIANZHIKRYLOV_BLOCK_ROW_MAJOR;
        m.memory_space=XIANZHIKRYLOV_HOST;m.device_id=-1;m.rows=m.columns=a.h.rows;
        m.entries=a.h.entries;
        if(format==XIANZHIKRYLOV_CSR){
            m.row_offsets=input(a.row);m.column_indices=input(a.col);
            m.values=a.h.scalar==1?input(a.a):input(a.za);
        }else if(format==XIANZHIKRYLOV_MSR&&block==1&&a.h.scalar==1){
            row.push_back(0);values.resize(a.h.rows);
            for(size_t i=0;i<a.h.rows;++i){
                bool diagonal=false;
                for(int k=a.row[i];k<a.row[i+1];++k)
                    if(a.col[k]==int(i)){values[i]=a.a[k];diagonal=true;}
                    else {col.push_back(a.col[k]);off.push_back(a.a[k]);}
                if(!diagonal)return false;
                row.push_back(col.size());
            }
            m.entries=col.size();m.row_offsets=input(row);m.column_indices=input(col);
            m.values=input(values);m.offdiagonal_values=input(off);
        }else if(format==XIANZHIKRYLOV_LDU&&a.h.scalar==1){
            values.resize(a.h.rows);
            for(size_t i=0;i<a.h.rows;++i){
                bool diagonal=false;
                for(int k=a.row[i];k<a.row[i+1];++k){
                    int j=a.col[k];
                    if(j==int(i)){values[i]=a.a[k];diagonal=true;}
                    if(j==int(i))continue;
                    auto p=std::lower_bound(a.col.begin()+a.row[j],a.col.begin()+a.row[j+1],int32_t(i));
                    if(p==a.col.begin()+a.row[j+1]||*p!=int32_t(i))return false;
                    if(j<int(i))continue;
                    lo.push_back(i);hi.push_back(j);upper.push_back(a.a[k]);lower.push_back(a.a[p-a.col.begin()]);
                }
                if(!diagonal)return false;
            }
            m.entries=lo.size();m.values=input(values);m.lower_address=input(lo);m.upper_address=input(hi);
            m.lower_values=input(lower);m.upper_values=input(upper);
        }else if(format==XIANZHIKRYLOV_BSR&&block==4&&a.h.scalar==1&&a.h.rows%4==0){
            brow.push_back(0);m.rows=m.columns=a.h.rows/4;
            for(size_t i=0;i<m.rows;++i){
                std::map<uint64_t,std::array<double,16>> blocks;
                for(int r=0;r<4;++r)for(int k=a.row[i*4+r];k<a.row[i*4+r+1];++k)
                    blocks[a.col[k]/4][r*4+a.col[k]%4]=a.a[k];
                bool diagonal=false;
                for(const auto& item:blocks){
                    if(item.first==i){bdiag.push_back(bcol.size());diagonal=true;}
                    bcol.push_back(item.first);values.insert(values.end(),item.second.begin(),item.second.end());
                }
                if(!diagonal)return false;brow.push_back(bcol.size());
            }
            m.index_type=XIANZHIKRYLOV_INDEX_I64;m.entries=bcol.size();
            m.row_offsets=input(brow);m.column_indices=input(bcol);m.diagonal_positions=input(bdiag);m.values=input(values);
        }else {std::fprintf(stderr,"Unsupported sample conversion\n");return false;}
        auto& b=system.rhs;b.abi_version=XIANZHIKRYLOV_DATA_ABI;b.struct_size=sizeof(b);
        b.scalar_type=a.h.scalar;b.layout=XIANZHIKRYLOV_RHS_MAJOR;b.memory_space=XIANZHIKRYLOV_HOST;
        b.device_id=-1;b.rows=a.h.rows;b.rhs_count=a.h.rhs_count;b.leading_dimension=a.h.rows;
        b.data=a.h.scalar==1?input(a.b):input(a.zb);
        return true;
    }
};
struct Solution {
    xianzhikrylov_solution descriptor{};
    std::vector<double> x,low;
    std::vector<xianzhikrylov_complex> z;
    Solution(const Matrix& a,bool dd=false){
        auto& d=descriptor;d.abi_version=XIANZHIKRYLOV_DATA_ABI;d.struct_size=sizeof(d);
        d.scalar_type=dd?XIANZHIKRYLOV_DD:a.h.scalar;d.layout=XIANZHIKRYLOV_RHS_MAJOR;
        d.memory_space=XIANZHIKRYLOV_HOST;d.device_id=-1;d.rows=a.h.rows;
        d.rhs_count=a.h.rhs_count;d.leading_dimension=a.h.rows;
        if(a.h.scalar==1){x.resize(d.rows*d.rhs_count);d.data={x.data(),x.size()*8};}
        else {z.resize(d.rows*d.rhs_count);d.data={z.data(),z.size()*sizeof(z[0])};}
        if(dd){low.resize(x.size());d.low={low.data(),low.size()*8};}
    }
};
inline bool verify(const Matrix& a,const Solution& x,double tolerance=1e-8){
    bool all=true;
    for(unsigned rhs=0;rhs<a.h.rhs_count;++rhs){
        long double rr=0,bb=0,maximum=0;bool finite=true;
        for(size_t i=0;i<a.h.rows;++i){
            const size_t q=size_t(rhs)*a.h.rows+i;
            std::complex<long double> residual,bi;
            if(a.h.scalar==1)bi={a.b[q],0};else bi={a.zb[q].real,a.zb[q].imag};
            residual=bi;
            for(int k=a.row[i];k<a.row[i+1];++k){
                const size_t p=size_t(rhs)*a.h.rows+a.col[k];
                if(a.h.scalar==1)residual-=static_cast<long double>(a.a[k])*(static_cast<long double>(x.x[p])+(x.low.empty()?0.L:x.low[p]));
                else residual-=std::complex<long double>(a.za[k].real,a.za[k].imag)*std::complex<long double>(x.z[p].real,x.z[p].imag);
            }
            const long double r=std::abs(residual);finite=finite&&std::isfinite(r);
            rr+=std::norm(residual);bb+=std::norm(bi);maximum=std::max(maximum,r);
        }
        const long double relative=bb>0?std::sqrt(rr/bb):std::sqrt(rr);
        bool pass=finite&&(a.h.scalar==2?maximum<tolerance:relative<=tolerance);
        std::printf("Independent residual RHS %u: relative_l2=%.12Le absmax=%.12Le %s\n",rhs+1,relative,maximum,pass?"PASS":"FAIL");
        all=all&&pass;
    }
    return all;
}
inline std::filesystem::path root(const char* executable){
    return std::filesystem::canonical(executable).parent_path();
}
}
