#include <iostream>
#include "openfhe.h"
#include <vector>
#include <cstdlib>
#include <cmath>

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

Ciphertext<DCRTPoly> babystep_polynomial(std::vector<Ciphertext<DCRTPoly>> bs , uint32_t skip, Plaintext coefficient, CryptoContext<DCRTPoly> cc){
    Ciphertext<DCRTPoly> Res;
    for(uint32_t i = skip+bs.size()-1; i > skip; i--){
        Ciphertext<DCRTPoly> temp = cc->EvalMult(bs[i-skip],coefficient->GetCKKSPackedValue()[i].real());
        temp = cc->Rescale(temp);
        if(i == skip+bs.size()-1){
            Res = temp;
        }
        else Res = cc->EvalAdd(Res, temp);
    }
    Res = cc->EvalAdd(coefficient->GetCKKSPackedValue()[skip].real(), Res);

    return Res;   
}

Ciphertext<DCRTPoly> bsgs(uint32_t bs, uint32_t degree, Ciphertext<DCRTPoly> ct, Plaintext coefficient, CryptoContext<DCRTPoly> cc){
    std::vector<Ciphertext<DCRTPoly>> baby_step = compute_powers(bs, ct, cc);
    Ciphertext<DCRTPoly> input = baby_step.back();
    baby_step.pop_back();
    uint32_t gs = degree/bs;
    std::vector<Ciphertext<DCRTPoly>> giant_step = compute_powers(gs, input, cc);

    Ciphertext<DCRTPoly> Res;
    for(uint32_t i =0; i < gs; i++){
        Ciphertext<DCRTPoly> temp = babystep_polynomial(baby_step, i*bs, coefficient,cc);
        if(i ==0 ){
            Res = temp;
        }
        else{
            temp = cc->EvalMult(temp, giant_step[i]);
            temp = cc->Rescale(temp);
            Res = cc->EvalAdd(temp, Res);
        }
    }

    if(bs*gs < degree){
        Ciphertext<DCRTPoly> Res1;
        for(uint32_t i = degree, j = degree-bs*gs; i > bs*gs && j > 0; i--, j--){
            Ciphertext<DCRTPoly> temp = cc->EvalMult(coefficient->GetCKKSPackedValue()[i].real(), baby_step[j]);
            temp = cc->Rescale(temp);
            if(i == degree){
                Res1 = temp;
            }
            else Res1 = cc->EvalAdd(temp, Res1);
        }
        Res1 = cc->EvalAdd(Res1, coefficient->GetCKKSPackedValue()[bs*gs].real());
        Res1 = cc->EvalMult(Res1, giant_step[gs]);
        Res1 = cc->Rescale(Res1);

        Res = cc->EvalAdd(Res, Res1);
    }
    
    return Res; 
}

double getRandomNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    return dis(gen);
}

int main(){
    CryptoContext<DCRTPoly> cc = GenerateCKKSContext();
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
    for(size_t i=0; i<32768; i++){
        double cur = getRandomNumber();
        x.push_back(cur);
    }
    std::cout <<"input : ";
    for(int i=0; i<10; i++){
        std::cout <<x[i] << " ";
    }
    std::cout <<"\n";
    uint32_t degree = value.size()-1;
    Plaintext co = cc->MakeCKKSPackedPlaintext(value);
    Plaintext pt = cc->MakeCKKSPackedPlaintext(x);
    auto x1 = cc->Encrypt(keys.publicKey,pt);
    x1 = cc->EvalMult(x1,0.5);
    x1 = cc->Rescale(x1);
    uint32_t bs = 4;

    Ciphertext<DCRTPoly> res = cc->EvalAdd(x1, bsgs(bs, degree, x1, co, cc));
    Plaintext result;
    std::cout.precision(8);
    cc->Decrypt(res, keys.secretKey, &result);
    result->SetLength(32768);
    std::cout << std::endl;

    for(int i=0; i<10; i++){
        std::cout << result->GetCKKSPackedValue()[i].real() << " ";
    }
    std::cout <<"\n";
}
