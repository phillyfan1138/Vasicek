#ifndef __VASICEK_H_INCLUDED
#define __VASICEK_H_INCLUDED    
#include "FunctionalUtilities.h"
#include <vector>
namespace vasicek{   
    template<typename Alpha, typename Tau>
    auto helpComputeMoments(const Alpha& alpha, const Tau& tau){ //hleper function since called so much
        return (1-exp(-alpha*tau))/alpha;
    }
    template<typename Rho, typename Sigma, typename Alpha>
    auto crossMultiply(const Rho& rho, const Sigma& sigma1, const Sigma& sigma2, const Alpha& alpha1, const Alpha& alpha2){
        return (rho*sigma1*sigma2)/(alpha1*alpha2);
    }
    /**This is the expectation of the O-U process Z when A is diagonal; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    */
    template<typename GetElement, typename Array, typename Index, typename Tau>
    auto computeExpectation(const Array& y0, const Array& alpha, const Array& beta, const Index& sizeOfY0, const Tau& tau, const GetElement& getElement){
        return futilities::for_each_parallel(0, sizeOfY0, [&](const auto& index){
            return (getElement(y0[index])-getElement(beta[index]))*exp(-getElement(alpha[index])*tau)+getElement(beta[index]);
        });
    }
    /**This is the expectation of the O-U process Z when A is diagonal; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    */
    template<typename Array, typename Index, typename Tau>
    auto computeExpectation(const Array& y0, const Array& alpha, const Array& beta, const Index& sizeOfY0, const Tau& tau){
        return computeExpectation(y0, alpha, beta, sizeOfY0, tau, [](const auto& val){
            return val;
        });
    }
    /**
        This is the expectation of the O-U process Z when A is diagonal; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
        Long run expectation is one.
    */
    template<typename GetElement, typename Array, typename Index, typename Tau>
    auto computeExpectationLongRunOne(const Array& y0, const Array& alpha, const Index& sizeOfY0, const Tau& tau, const GetElement& getElement){
        return futilities::for_each_parallel(0, sizeOfY0, [&](const auto& index){
            return (getElement(y0[index])-1)*exp(-getElement(alpha[index])*tau)+1;
        });
    }
    /**
        This is the expectation of the O-U process Z when A is diagonal; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
        Long run expectation is one.
    */
    template<typename Array, typename Tau>
    auto computeExpectationLongRunOne(const Array& y0, const Array& alpha, const Tau& tau){
         return computeExpectationLongRunOne(y0, alpha, tau, [](const auto& val){
            return val;
        });
    }
    /**This is the expectation of the integral of the O-U process Z; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    */
    template<typename GetElement, typename Index, typename Array, typename Tau>
    auto computeIntegralExpectation(const Array& y0, const Array& alpha, const Array& beta, const Index& sizeOfY0, const Tau& tau, const GetElement& getElement){
        return futilities::for_each_parallel(0, sizeOfY0, [&](const auto& index){
            return (getElement(y0[index])-getElement(beta[index]))*helpComputeMoments(getElement(alpha[index]), tau)+getElement(beta[index])*tau;
        });
    }
    /**This is the expectation of the integral of the O-U process Z; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    */
    template<typename Index, typename Array, typename Tau>
    auto computeIntegralExpectation(const Array& y0, const Array& alpha, const Array& beta, const Index& sizeOfY0, const Tau& tau){
        return computeIntegralExpectation(y0, alpha, beta, sizeOfY0, tau, [](const auto& val){
            return val;
        });
    }
    /**This is the expectation of the integral of the O-U process Z; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    Long run expectation is one
    */
    template<typename GetElement, typename Index, typename Array, typename Tau>
    auto computeIntegralExpectationLongRunOne(const Array& y0, const Array& alpha, const Index& sizeOfY0, const Tau& tau, const GetElement& getElement){
        return futilities::for_each_parallel(0, sizeOfY0, [&](const auto& index){
            return (getElement(y0[index])-1)*helpComputeMoments(getElement(alpha[index]), tau)+tau;
        });
    }
    /**This is the expectation of the integral of the O-U process Z; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    Long run expectation is one
    */
    template<typename Index, typename Array, typename Alpha, typename Tau>
    auto computeIntegralExpectationLongRunOne(const Array& y0, const Array& alpha, const Index& sizeOfY0, const Tau& tau){
        return computeIntegralExpectationLongRunOne(y0, alpha, sizeOfY0, tau, [](const auto& val){
            return val;
        });
    }


    /**
    Computes the variance/covariance of the integral of a multivariate vasicek process; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    */
    template<typename GetElement,typename Array, typename Array2d, typename Tau, typename Index>
    auto computeIntegralVarianceVasicek(const Array& alpha, const Array& sigma, const Array2d& rho, const Index& sizeOfY0, const Tau& tau, const GetElement& getElement){
        return futilities::for_each_parallel(0, sizeOfY0, [&](const auto& indexI){
            auto ai=helpComputeMoments(getElement(alpha[indexI]), tau);
            return futilities::for_each_parallel(0, sizeOfY0, [&](const auto& indexJ){
                auto aj=helpComputeMoments(getElement(alpha[indexJ]), tau);
                return crossMultiply(getElement(rho[indexI][indexJ]), getElement(sigma[indexI]), getElement(sigma[indexJ]), getElement(alpha[indexI]), getElement(alpha[indexJ]))*(tau-ai-aj+helpComputeMoments(getElement(alpha[indexI])+getElement(alpha[indexJ]), tau));
            });
        });
    }
    /**
    Computes the variance/covariance of the integral of a multivariate vasicek process; see http://danielhstahl.com/static/media/CreditRiskExtensions.143b963f.pdf
    */
    template<typename Array, typename Array2d, typename Tau, typename Index>
    auto computeIntegralVarianceVasicek(const Array& alpha, const Array& sigma, const Array2d& rho, const Index& sizeOfY0, const Tau& tau){
        return computeIntegralVarianceVasicek(alpha, sigma, rho, sizeOfY0, tau, [](const auto& val){
            return val;
        });
    }

    /**Computes the expectation of a the exponential of a weighted combination of the multidemensional integrated vasicek process*/
    template<typename Expectation, typename Variance, typename Number>
    auto getVasicekMFGFn(const std::vector<Expectation>& expectation , const std::vector< std::vector<Variance> >& variance){
        int m=expectation.size();
        return [m, &expectation, &variance](const auto& v){ //ocnst std::vector<std::complex<Number> > &v, 
            return exp(futilities::sum(0, m, [&](const auto& index){
                return v[index]*expectation[index];
            })+futilities::sum(0, m, [&](const auto& indexI){
                return futilities::sum(0, m, [&](const auto& indexJ){
                    return v[indexI]*v[indexJ]*variance[indexI][indexJ];
                });
            })*.5);
        };
    }
}
#endif