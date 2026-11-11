#pragma once

// Included inside pico_fusion after magnetic types. Bounded calibration-only learner.
struct MagFit {
    MagCalibration calibration;
    double radius=0,rms=0,coverage=0; unsigned samples=0;
    double minEigen=0,meanDirection=0,halfDifference=0,envelopeDifference=0;
    int model=0; // 0 sphere, 1 diagonal, 2 full
    bool ready=false; unsigned qualityFailures=0;
};
inline bool MagSolve(double a[9][10],int n,double *x) {
    for(int k=0;k<n;++k) {
        int p=k;for(int i=k+1;i<n;++i)if(std::fabs(a[i][k])>std::fabs(a[p][k]))p=i;
        if(std::fabs(a[p][k])<1e-9)return false;
        for(int j=k;j<=n;++j)std::swap(a[k][j],a[p][j]);
        double d=a[k][k];for(int j=k;j<=n;++j)a[k][j]/=d;
        for(int i=0;i<n;++i)if(i!=k){d=a[i][k];for(int j=k;j<=n;++j)a[i][j]-=d*a[k][j];}
    }
    for(int i=0;i<n;++i)x[i]=a[i][n];
    return true;
}
struct MagEllipsoid {
    std::vector<Vec3> points;
    MagFit fit, attempted;
    uint64_t received=0,seen=0,rejected=0,rateLimited=0,attempts=0,lastSolvedAttempt=0; double last=0;
    const char *fitStatus="insufficient_samples";
    bool Observe(double t,Vec3 b) {
        ++received;
        if(t<=last || !std::isfinite(Norm(b)) || Norm(b)>150){++rejected;return false;}
        if(t-last<20e6){++rateLimited;return false;} last=t; ++seen;
        if(fit.ready && std::fabs(Norm(MagCalibration::Matrix(fit.calibration.soft,Sub(b,fit.calibration.offset)))-fit.radius)>8){++rejected;return false;}
        if(points.size()==2048)points.erase(points.begin());
        points.push_back(b);
        if(points.size()<240 || seen%64)return false;
        return Refit();
    }
    bool Refit(bool diagonal=false) {
        ++attempts; fitStatus="algebraic_or_physical_rejection";
        // Algebraic full SPD ellipsoid, x^T A x + d^T x = 1, units normalized to 50 uT.
        // Iteratively reject radial outliers; acceptance also checks spherical direction coverage.
        std::vector<bool> use(points.size(),true);MagFit candidate;
        for(int pass=0;pass<3;++pass) {
            double a[9][10]={},v[9]={};unsigned n=0;
            for(size_t i=0;i<points.size();++i)if(use[i]){
                Vec3 b=Scale(points[i],.02);double f[9]={b.x*b.x,b.y*b.y,b.z*b.z,2*b.x*b.y,2*b.x*b.z,2*b.y*b.z,b.x,b.y,b.z};
                for(int r=0;r<9;++r){for(int c=0;c<9;++c)a[r][c]+=f[r]*f[c];
                a[r][9]+=f[r];}++n;
            }
            if(diagonal){int ix[6]={0,1,2,6,7,8};double d[9][10]={},x[9]={};for(int r=0;r<6;++r){for(int c=0;c<6;++c)d[r][c]=a[ix[r]][ix[c]];
            d[r][6]=a[ix[r]][9];}if(!MagSolve(d,6,x))return false;
            for(int r=0;r<6;++r)v[ix[r]]=x[r];}
            if(n<240 || (!diagonal && !MagSolve(a,9,v))){fitStatus="singular_or_too_few_inliers";return false;}
            double h[9][10]={};h[0][0]=v[0];h[1][1]=v[1];h[2][2]=v[2];h[0][1]=h[1][0]=v[3];h[0][2]=h[2][0]=v[4];h[1][2]=h[2][1]=v[5];
            for(int i=0;i<3;++i)h[i][3]=-.5*v[i+6];
            double center[3];if(!MagSolve(h,3,center))return false;
            double k=1-.5*(v[6]*center[0]+v[7]*center[1]+v[8]*center[2]);if(k<=0)return false;
            double m[3][3]={{v[0]/k,v[3]/k,v[4]/k},{v[3]/k,v[1]/k,v[5]/k},{v[4]/k,v[5]/k,v[2]/k}};
            double eig[3][3]={{1,0,0},{0,1,0},{0,0,1}};
            for(int it=0;it<32;++it){int p=0,q=1;for(int r=0;r<3;++r)for(int c=r+1;c<3;++c)if(std::fabs(m[r][c])>std::fabs(m[p][q])){p=r;q=c;}
                if(std::fabs(m[p][q])<1e-12)break;
                double ang=.5*std::atan2(2*m[p][q],m[q][q]-m[p][p]),cs=std::cos(ang),sn=std::sin(ang);
                for(int r=0;r<3;++r){double x=m[r][p],y=m[r][q];m[r][p]=cs*x-sn*y;m[r][q]=sn*x+cs*y;}
                for(int c=0;c<3;++c){double x=m[p][c],y=m[q][c];m[p][c]=cs*x-sn*y;m[q][c]=sn*x+cs*y;}
                for(int r=0;r<3;++r){double x=eig[r][p],y=eig[r][q];eig[r][p]=cs*x-sn*y;eig[r][q]=sn*x+cs*y;}
            }
            double mn=1e9,mx=0,det=1;for(int i=0;i<3;++i){mn=std::fmin(mn,m[i][i]);mx=std::fmax(mx,m[i][i]);det*=m[i][i];}
            if(mn<=0 || mx/mn>9){fitStatus="non_spd_or_axis_ratio";return false;}
            double radius=50/std::pow(det,1./6);if(radius<20||radius>70){fitStatus="radius_out_of_range";return false;}
            candidate.calibration.offset={50*center[0],50*center[1],50*center[2]};
            if(Norm(candidate.calibration.offset)>100)return false;
            for(int r=0;r<3;++r)for(int c=0;c<3;++c){double s=0;for(int j=0;j<3;++j)s+=eig[r][j]*std::sqrt(m[j][j])*eig[c][j];
            candidate.calibration.soft[3*r+c]=radius/50*s;}
            unsigned bins=0,count=0;double error=0; Vec3 lo{1,1,1},hi{-1,-1,-1};
            for(size_t i=0;i<points.size();++i){Vec3 b=MagCalibration::Matrix(candidate.calibration.soft,Sub(points[i],candidate.calibration.offset));double norm=Norm(b),e=norm-radius;
                use[i]=std::fabs(e)<std::fmax(3.,.08*radius);if(!use[i])continue;
                ++count;error+=e*e;b=Scale(b,1/norm);bins|=1u<<((b.x>0?1:0)+(b.y>0?2:0)+(b.z>0?4:0));
                lo={std::fmin(lo.x,b.x),std::fmin(lo.y,b.y),std::fmin(lo.z,b.z)};hi={std::fmax(hi.x,b.x),std::fmax(hi.y,b.y),std::fmax(hi.z,b.z)};
            }
            unsigned occupied=0;for(unsigned b=bins;b;b>>=1)occupied+=b&1;
            candidate.radius=radius;candidate.rms=count?std::sqrt(error/count):1e9;candidate.samples=count;candidate.coverage=100.*occupied/8;
            candidate.qualityFailures=(count<240||count<points.size()*.85?1u:0u)|(!(candidate.rms<1.5)?2u:0u)|(occupied!=8?4u:0u)|(!(lo.x<-.8&&lo.y<-.8&&lo.z<-.8&&hi.x>.8&&hi.y>.8&&hi.z>.8)?8u:0u);
            candidate.ready=candidate.qualityFailures==0;
        }
        attempted=candidate; lastSolvedAttempt=attempts; fitStatus=candidate.ready?"accepted":"quality_rejection";
        if(!candidate.ready)return false;
        // Mapping is a separate experiment, never invented by a magnitude fit.
        candidate.calibration.valid=false;fit=candidate;return true;
    }
};
#include "mag_sphere.h"
struct MagMapping {
    Vec3 sums[48]{};double energy[48]{};unsigned count=0;int best=-1;double agreement=0,margin=0;
    static void Matrix(int index,double *m){int perm[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};for(int i=0;i<9;++i)m[i]=0;
    for(int r=0;r<3;++r)m[3*r+perm[index/8][r]]=(index&(1<<r))?-1.:1.;}
    void Observe(Vec3 calibrated,Quat bodyToWorld){
        double n=Norm(calibrated);if(n<20||n>70)return;
        for(int i=0;i<48;++i){double m[9];Matrix(i,m);Vec3 w=Rotate(bodyToWorld,Scale(MagCalibration::Matrix(m,calibrated),1/n));sums[i]=Add(sums[i],w);energy[i]+=Norm(w)*Norm(w);}
        ++count;double first=-1,second=-1;int winner=-1;
        // A global sign is physically unobservable without a known Earth field direction.
        // Report the sign pair as such; do not mark the mount valid.
        for(int i=0;i<48;++i)if(!(i&4)){double score=Norm(sums[i])/count;if(score>first){second=first;first=score;winner=i;}else second=std::fmax(second,score);}
        best=winner;agreement=first;margin=first-second;
    }
};
