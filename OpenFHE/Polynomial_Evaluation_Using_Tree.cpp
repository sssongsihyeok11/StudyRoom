#include <iostream>
#include "openfhe.h"
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <random>

using namespace lbcrypto;

uint depth;
CryptoContext<DCRTPoly> GenerateCKKSContext(){    
    SecurityLevel securityLevel = HEStd_NotSet;
    uint32_t dcrtBits           = 59;
    uint32_t ringDim            = 1 << 16;
    uint32_t batchSize          = 1 << 15;

    CCParams<CryptoContextCKKSRNS> parameters;
    depth = 10;
    parameters.SetMultiplicativeDepth(depth);
    parameters.SetScalingModSize(dcrtBits);
    parameters.SetBatchSize(batchSize);
    parameters.SetSecurityLevel(securityLevel);
    parameters.SetRingDim(ringDim);
    parameters.SetFirstModSize(60);
    parameters.SetScalingTechnique(FLEXIBLEAUTO);

    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(ADVANCEDSHE);
    cc->Enable(LEVELEDSHE);
    cc->Enable(FHE);

    return cc;
}

// Build vector of Ciphertext until degree of n 
std::vector<Ciphertext<DCRTPoly>> compute_powers(int degree, Ciphertext<DCRTPoly> ctx, CryptoContext<DCRTPoly> cc){
    std::vector<Ciphertext<DCRTPoly>> Res(degree +1);
    Res[1] = ctx;

    std::vector<int> levels(degree+1, 0);

    for(int i = 2; i<=degree; i+=1){
        int min_level = i;
        int cand = -1;
        for(int j=1; j<=i/2; j+=1){
            int k = i-j;
            int new_level = std::max(levels[j],levels[k])+1;
            if(new_level < min_level){
                cand = j;
                min_level = new_level;
            }
        }

        levels[i] = min_level;
       
        Res[i] = cc->EvalMult(Res[cand], Res[i-cand]);
        Res[i] = cc->Rescale(Res[i]);
    }

    return Res;
}

// approximate abs(x)/2, 
// implement abs(x)/2 + x/2
Ciphertext<DCRTPoly> ReLU_Evaluation_Using_Tree(int degree, Plaintext &coefficient, Ciphertext<DCRTPoly> ctx, CryptoContext<DCRTPoly> cc){
    
    std::vector<Ciphertext<DCRTPoly>> power = compute_powers(degree, ctx,cc);
    Ciphertext<DCRTPoly> Res = cc->EvalMult(power[degree], coefficient->GetCKKSPackedValue()[degree].real());
    Res = cc->Rescale(Res);
    for(int i=degree-1; i>0; i--){
        auto temp = cc->EvalMult(coefficient->GetCKKSPackedValue()[i].real(),power[i]);
        temp = cc->Rescale(temp);
        Res = cc->EvalAdd(Res, temp);
    }
    Res = cc->EvalAdd(Res, coefficient->GetCKKSPackedValue()[0].real());
    //Res = cc->EvalMult(Res,0.5);
    //Res = cc->Rescale(Res);
    auto coef_1 = cc->EvalMult(ctx, 0.5);
    coef_1 = cc->Rescale(coef_1);
    Res = cc->EvalAdd(coef_1,Res);
    
    std::cout << "ReLU level : " << Res->GetLevel() << std::endl;

    return Res;
}

double getRandomNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    return dis(gen);
}

void ReLU(){
    
    CryptoContext<DCRTPoly> cc = GenerateCKKSContext();

    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    cc->Enable(ADVANCEDSHE);

    KeyPair<DCRTPoly> keys = cc->KeyGen();
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalSumKeyGen(keys.secretKey);

    std::vector<double> x;
    std::vector<double> value = {
        8.63372843370292/2,	
        0.00116330178628758/2,	
        -23.7867582162146/2,	
        -0.00220414661623988/2,	
        24.5378730273638/2,	
        0.00136138212017186/2,	
        -12.0283466043393/2,	
        -0.000302528792559805/2,	
        3.60849721256207/2,	
        1.74535514789870e-05/2,	
        0.0555152355379635/2
    };
    std::reverse(value.begin(), value.end());
    int degree = value.size()-1;
    Plaintext co = cc->MakeCKKSPackedPlaintext(value);

    for(size_t i=0; i<32768; i++){
        double cur = getRandomNumber();
        x.push_back(cur);
    }

    std::cout <<"input : ";
    for(int i=0; i<10; i++){
        std::cout <<x[i] << " ";
    }
    Plaintext ptxt = cc->MakeCKKSPackedPlaintext(x);
    
    auto x1 = cc->Encrypt(keys.publicKey,ptxt);
    std::cout << "Start ReLU_Function \n";
    std::chrono::system_clock::time_point start_Function = std::chrono::system_clock::now();
    auto m = ReLU_Evaluation_Using_Tree(degree,co, x1,cc);
    std ::cout << "ReLU Level : " << m->GetLevel() << std::endl;
    std::cout << "End ReLU Function \n\n";
    std::chrono::duration<double> func_end_time = std::chrono::system_clock::now() - start_Function;

    std::cout << "Play Function :: " << func_end_time.count() << "\n";

    Plaintext result;
    std::cout.precision(8);
    cc->Decrypt(m, keys.secretKey, &result);
    result->SetLength(32768);
    std::cout << std::endl;
     for(int i=0; i<10; i++){
        std::cout << result->GetCKKSPackedValue()[i].real() << " ";
    }
}

int main(){
    ReLU();
}