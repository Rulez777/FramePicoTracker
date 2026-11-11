// Included inside pico_fusion. Calibration only; no orientation assumption.
inline double MagMedian(std::vector<double> v) {
    std::sort(v.begin(),v.end()); return v.empty()?0:v[v.size()/2];
}
inline double MagRms(const std::vector<Vec3>& p,const MagFit& f) {
    double sum=0;for(auto b:p){double e=Norm(MagCalibration::Matrix(f.calibration.soft,Sub(b,f.calibration.offset)))-f.radius;sum+=e*e;}
    return p.empty()?1e9:std::sqrt(sum/p.size());
}
inline MagFit MagSphere(const std::vector<Vec3>& p) {
    MagFit f;if(p.size()<120)return f;
    double a[9][10]={},x[9]={};
    for(auto b:p){b=Scale(b,.02);double v[4]={2*b.x,2*b.y,2*b.z,1};double y=(b.x*b.x+b.y*b.y+b.z*b.z);for(int r=0;r<4;++r){for(int c=0;c<4;++c)a[r][c]+=v[r]*v[c];
    a[r][4]+=v[r]*y;}}
    if(!MagSolve(a,4,x))return f;
    Vec3 center{50*x[0],50*x[1],50*x[2]};
    double radius=50*std::sqrt(std::fmax(0.,x[3]+x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));
    for(int it=0;it<15;++it){
        std::vector<double> errors;for(auto b:p)errors.push_back(std::fabs(Norm(Sub(b,center))-radius));
        double cutoff=std::fmax(1.,4.685*1.4826*MagMedian(errors));
        double h[9][10]={},step[9]={};
        for(auto b:p){Vec3 d=Sub(center,b);double n=Norm(d);if(n<1e-9)continue;
        double e=n-radius,u=e/cutoff;if(std::fabs(u)>=1)continue;
        double w=(1-u*u)*(1-u*u),v[4]={d.x/n,d.y/n,d.z/n,-1};for(int r=0;r<4;++r){for(int c=0;c<4;++c)h[r][c]+=w*v[r]*v[c];
        h[r][4]-=w*v[r]*e;}}
        if(!MagSolve(h,4,step))return f;
        center=Add(center,{step[0],step[1],step[2]});radius+=step[3];
    }
    f.calibration.offset=center;
    if(!std::isfinite(radius)||radius<20||radius>70||Norm(center)>100)return f;
    f.radius=radius;
    std::vector<double> errors;for(auto b:p)errors.push_back(std::fabs(Norm(Sub(b,center))-radius));
    double cutoff=std::fmin(8.,std::fmax(3.,3*1.4826*MagMedian(errors)));
    unsigned bins=0;double sum=0,cov[3][3]={};Vec3 mean{};
    for(auto b:p){Vec3 d=Sub(b,center);double n=Norm(d),e=n-radius;if(std::fabs(e)>cutoff||n<1)continue;
        ++f.samples;sum+=e*e;d=Scale(d,1/n);mean=Add(mean,d);double v[3]={d.x,d.y,d.z};for(int r=0;r<3;++r)for(int c=0;c<3;++c)cov[r][c]+=v[r]*v[c];
        bins|=1u<<((d.x>0?1:0)+(d.y>0?2:0)+(d.z>0?4:0));
    }
    if(!f.samples)return f;
    mean=Scale(mean,1./f.samples);double mv[3]={mean.x,mean.y,mean.z};
    for(int r=0;r<3;++r)for(int c=0;c<3;++c)cov[r][c]=cov[r][c]/f.samples-mv[r]*mv[c];
    for(int it=0;it<32;++it){int r=0,c=1;for(int i=0;i<3;++i)for(int j=i+1;j<3;++j)if(std::fabs(cov[i][j])>std::fabs(cov[r][c])){r=i;c=j;}
        if(std::fabs(cov[r][c])<1e-12)break;
        double ang=.5*std::atan2(2*cov[r][c],cov[c][c]-cov[r][r]),cs=std::cos(ang),sn=std::sin(ang);
        for(int i=0;i<3;++i){double u=cov[i][r],v=cov[i][c];cov[i][r]=cs*u-sn*v;cov[i][c]=sn*u+cs*v;}
        for(int i=0;i<3;++i){double u=cov[r][i],v=cov[c][i];cov[r][i]=cs*u-sn*v;cov[c][i]=sn*u+cs*v;}
    }
    f.minEigen=std::fmin(cov[0][0],std::fmin(cov[1][1],cov[2][2]));f.meanDirection=Norm(mean);
    unsigned occupied=0;for(;bins;bins>>=1)occupied+=bins&1;
    f.coverage=12.5*occupied;f.rms=std::sqrt(sum/f.samples);
    f.qualityFailures=(f.samples<120||f.samples<p.size()*.75?1u:0u)|(f.rms>=3?2u:0u)|((occupied<5||f.minEigen<.08||f.meanDirection>.65)?4u:0u);
    if(f.radius<30||f.radius>65)f.qualityFailures|=8;
    f.ready=!f.qualityFailures;return f;
}
struct MagLearner {
    std::vector<Vec3> points;
    std::vector<double> times;
    MagFit fit,attempted;
    uint64_t received=0,seen=0,rejected=0,rateLimited=0,attempts=0,lastSolvedAttempt=0;double last=0;
    const char* fitStatus="insufficient_samples";
    bool Observe(double t,Vec3 b){
        ++received;if(t<=last||!std::isfinite(Norm(b))||Norm(b)>150){++rejected;return false;}
        if(t-last<20e6){++rateLimited;return false;}last=t;++seen;
        if(fit.ready&&std::fabs(Norm(MagCalibration::Matrix(fit.calibration.soft,Sub(b,fit.calibration.offset)))-fit.radius)>12){++rejected;return false;}
        bool novel=true;for(auto p:points)if(Norm(Sub(p,b))<1.5){novel=false;break;}
        if(novel){if(points.size()==4096){points.erase(points.begin());times.erase(times.begin());}points.push_back(b);times.push_back(t);}
        if(points.size()<240||seen%64)return false;
        return Refit();
    }
    bool Refit(){
        ++attempts;MagFit f=MagSphere(points);fitStatus="sphere_geometry_or_radius";
        if(!f.radius){fit.ready=false;attempted=f;return false;}lastSolvedAttempt=attempts;if(f.samples<240)f.qualityFailures|=1; if(f.radius<30||f.radius>65)f.qualityFailures|=8;
        // One-percent coordinate quantiles prevent isolated spikes defining extrema.
        std::vector<double> axes[3];for(auto b:points){axes[0].push_back(b.x);axes[1].push_back(b.y);axes[2].push_back(b.z);}
        double env[3];for(int i=0;i<3;++i){std::sort(axes[i].begin(),axes[i].end());size_t k=axes[i].size()/100;env[i]=.5*(axes[i][k]+axes[i][axes[i].size()-1-k]);}
        Vec3 envelope{env[0],env[1],env[2]};
        f.envelopeDifference=Norm(Sub(f.calibration.offset,envelope));
        std::vector<Vec3> left,right;
        for(size_t i=0;i<points.size();++i){bool first=i<points.size()/2;(first?left:right).push_back(points[i]);}
        auto l=MagSphere(left),r=MagSphere(right);
        f.halfDifference=(l.radius&&r.radius)?Norm(Sub(l.calibration.offset,r.calibration.offset)):1e9;
        bool complete=f.coverage==100&&f.minEigen>.20&&f.meanDirection<.20;
        // Envelope is diagnostic only: partial coverage biases its centre.
        if(complete){
            std::vector<Vec3> train,test;for(size_t i=0;i<points.size();++i)(i%2?test:train).push_back(points[i]);
            double baseline=MagRms(test,MagSphere(train));
            for(int model=1;model<=2;++model){MagEllipsoid e;e.points=train;e.Refit(model==1);auto trial=e.attempted;
                double rms=MagRms(test,trial);
                if(trial.ready&&rms<baseline*.8&&baseline-rms>.2){
                    MagEllipsoid all;all.points=points;all.Refit(model==1);
                    if(all.attempted.ready){auto next=all.attempted;next.model=model;next.minEigen=f.minEigen;next.envelopeDifference=Norm(Sub(next.calibration.offset,envelope));next.qualityFailures=0;f=next;baseline=rms;}
                }
            }
        }
        if(f.model){MagEllipsoid a,b;a.points=left;b.points=right;a.Refit(f.model==1);b.Refit(f.model==1);l=a.attempted;r=b.attempted;}
        f.halfDifference=(l.radius&&r.radius)?Norm(Sub(l.calibration.offset,r.calibration.offset)):1e9;
        if(f.halfDifference>=6)f.qualityFailures|=16;
        if(f.radius<30||f.radius>65)f.qualityFailures|=8;
        if(f.rms>=3)f.qualityFailures|=2;
        f.ready=!f.qualityFailures;attempted=f;
        fitStatus=f.ready?(f.model==0?"accepted_sphere":f.model==1?"accepted_diagonal":"accepted_full"):(f.qualityFailures&16?"unstable_time_halves":f.qualityFailures&32?"envelope_disagreement":"sphere_coverage_or_rms");
        if(!f.ready){fit.ready=false;return false;}f.calibration.valid=false;f.calibration.radius=f.radius;fit=f;return true;
    }
};
