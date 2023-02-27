#ifndef FILESYSTEM_APP_CREATEUSER_H
#define FILESYSTEM_APP_CREATEUSER_H

#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "Utils.h"
#include "RSAEncryption.h"
#include "KeyEncrypter.h"
#include "FileEncrypter.h"

using namespace AppUtils;
using namespace AppConstants::DefaultDirectory;

class CreateUser {

private:
    RSAEncryption mRSAEncryption;
    KeyEncrypter mKeyEncrypter;
    FileEncrypter mFileEncrypter;

public:

    bool generateKeysForUser(string username_to_be_added, vector<string> paths, string username = "") {
        int ret = 0;
        RSA *r = NULL;
        BIGNUM *bne = NULL;
        BIO *bp_public = NULL, *bp_private = NULL, *bp_private_share_admin = NULL;

        int bits = 4096;
        unsigned long e = RSA_F4;
        string public_key_name = paths[0] + "/" + username_to_be_added + "_public.pem";
        string private_key_name = paths[1] + "/" + username_to_be_added + "_private.pem";

        // 1. generate rsa key
        bne = BN_new();
        ret = BN_set_word(bne, e);
        if (ret != 1) {
            goto free_all;
        }

        r = RSA_new();
        ret = RSA_generate_key_ex(r, bits, bne, NULL);
        if (ret != 1) {
            goto free_all;
        }

        // 2. save public key
        bp_public = BIO_new_file(public_key_name.c_str(), "w+");
        ret = PEM_write_bio_RSAPublicKey(bp_public, r);
        if (ret != 1) {
            goto free_all;
        }


        // 3. save private key
        bp_private = BIO_new_file(private_key_name.c_str(), "w+");
        ret = PEM_write_bio_RSAPrivateKey(bp_private, r, NULL, NULL, 0, NULL, NULL);

        if (username_to_be_added != ADMIN) {

            BIO* bio = BIO_new(BIO_s_mem());
            PEM_write_bio_RSAPrivateKey(bio, r, NULL, NULL, 0, NULL, NULL);

            char* buffer;
            long length = BIO_get_mem_data(bio, &buffer);

            string private_key_string(buffer, length);

            vector<string> paths = Utils::getPublicAndPrivateKeyPaths();

            string root_path = Utils::getRootDirPath(Utils::getPwdPath(), FILE_SYSTEM);
            string private_key_name_for_admin = paths[0] + "/" +
                                                Randomizer::getMetaValue(root_path, PRIVATE) + "/" +
                                                username_to_be_added + "_private.pem";
            string admin_public_key_name = paths[0] + "/" + ADMIN+"_public.pem";
            string aes_key_path = paths[0] + "/" + ADMIN + AES_KEY_EXT;            
            mFileEncrypter.encrypt_file_dyn(private_key_name_for_admin, private_key_string,  paths,  username);
            BIO_free(bio);
        }


        // 4. free
        free_all:
            BIO_free_all(bp_public);
            BIO_free_all(bp_private);
            BIO_free_all(bp_private_share_admin);
            RSA_free(r);
            BN_free(bne);

        return (ret == 1);
    }

};


#endif //FILESYSTEM_APP_CREATEUSER_H
