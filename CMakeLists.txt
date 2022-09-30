cmake_minimum_required(VERSION 3.13)

project(
	Unalix
	VERSION 0.1
	DESCRIPTION "ClearURLs implementation in C99"
	HOMEPAGE_URL "https://github.com/AmanoTeam/libunalix"
	LANGUAGES C
)

option(UNALIX_BUILD_TESTING "enable testing for Unalix" OFF)

cmake_policy(SET CMP0077 NEW)

include_directories(
	src
	submodules/bearssl/src
	submodules/bearssl/inc
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS ON)

# jansson
option(JANSSON_BUILD_DOCS OFF)
set(JANSSON_BUILD_SHARED_LIBS ON CACHE BOOL "")

# pcre2
option(PCRE2_SHOW_REPORT OFF)

add_compile_options(-Oz -flto=full -Wall -Wextra -g)

add_library(
	bearssl
	SHARED
	submodules/bearssl/src/settings.c
	submodules/bearssl/src/aead/ccm.c
	submodules/bearssl/src/aead/eax.c
	submodules/bearssl/src/aead/gcm.c
	submodules/bearssl/src/codec/ccopy.c
	submodules/bearssl/src/codec/dec16be.c
	submodules/bearssl/src/codec/dec16le.c
	submodules/bearssl/src/codec/dec32be.c
	submodules/bearssl/src/codec/dec32le.c
	submodules/bearssl/src/codec/dec64be.c
	submodules/bearssl/src/codec/dec64le.c
	submodules/bearssl/src/codec/enc16be.c
	submodules/bearssl/src/codec/enc16le.c
	submodules/bearssl/src/codec/enc32be.c
	submodules/bearssl/src/codec/enc32le.c
	submodules/bearssl/src/codec/enc64be.c
	submodules/bearssl/src/codec/enc64le.c
	submodules/bearssl/src/codec/pemdec.c
	submodules/bearssl/src/codec/pemenc.c
	submodules/bearssl/src/ec/ec_all_m15.c
	submodules/bearssl/src/ec/ec_all_m31.c
	submodules/bearssl/src/ec/ec_c25519_i15.c
	submodules/bearssl/src/ec/ec_c25519_i31.c
	submodules/bearssl/src/ec/ec_c25519_m15.c
	submodules/bearssl/src/ec/ec_c25519_m31.c
	submodules/bearssl/src/ec/ec_c25519_m62.c
	submodules/bearssl/src/ec/ec_c25519_m64.c
	submodules/bearssl/src/ec/ec_curve25519.c
	submodules/bearssl/src/ec/ec_default.c
	submodules/bearssl/src/ec/ec_keygen.c
	submodules/bearssl/src/ec/ec_p256_m15.c
	submodules/bearssl/src/ec/ec_p256_m31.c
	submodules/bearssl/src/ec/ec_p256_m62.c
	submodules/bearssl/src/ec/ec_p256_m64.c
	submodules/bearssl/src/ec/ec_prime_i15.c
	submodules/bearssl/src/ec/ec_prime_i31.c
	submodules/bearssl/src/ec/ec_pubkey.c
	submodules/bearssl/src/ec/ec_secp256r1.c
	submodules/bearssl/src/ec/ec_secp384r1.c
	submodules/bearssl/src/ec/ec_secp521r1.c
	submodules/bearssl/src/ec/ecdsa_atr.c
	submodules/bearssl/src/ec/ecdsa_default_sign_asn1.c
	submodules/bearssl/src/ec/ecdsa_default_sign_raw.c
	submodules/bearssl/src/ec/ecdsa_default_vrfy_asn1.c
	submodules/bearssl/src/ec/ecdsa_default_vrfy_raw.c
	submodules/bearssl/src/ec/ecdsa_i15_bits.c
	submodules/bearssl/src/ec/ecdsa_i15_sign_asn1.c
	submodules/bearssl/src/ec/ecdsa_i15_sign_raw.c
	submodules/bearssl/src/ec/ecdsa_i15_vrfy_asn1.c
	submodules/bearssl/src/ec/ecdsa_i15_vrfy_raw.c
	submodules/bearssl/src/ec/ecdsa_i31_bits.c
	submodules/bearssl/src/ec/ecdsa_i31_sign_asn1.c
	submodules/bearssl/src/ec/ecdsa_i31_sign_raw.c
	submodules/bearssl/src/ec/ecdsa_i31_vrfy_asn1.c
	submodules/bearssl/src/ec/ecdsa_i31_vrfy_raw.c
	submodules/bearssl/src/ec/ecdsa_rta.c
	submodules/bearssl/src/hash/dig_oid.c
	submodules/bearssl/src/hash/dig_size.c
	submodules/bearssl/src/hash/ghash_ctmul.c
	submodules/bearssl/src/hash/ghash_ctmul32.c
	submodules/bearssl/src/hash/ghash_ctmul64.c
	submodules/bearssl/src/hash/ghash_pclmul.c
	submodules/bearssl/src/hash/ghash_pwr8.c
	submodules/bearssl/src/hash/md5.c
	submodules/bearssl/src/hash/md5sha1.c
	submodules/bearssl/src/hash/mgf1.c
	submodules/bearssl/src/hash/multihash.c
	submodules/bearssl/src/hash/sha1.c
	submodules/bearssl/src/hash/sha2big.c
	submodules/bearssl/src/hash/sha2small.c
	submodules/bearssl/src/int/i15_add.c
	submodules/bearssl/src/int/i15_bitlen.c
	submodules/bearssl/src/int/i15_decmod.c
	submodules/bearssl/src/int/i15_decode.c
	submodules/bearssl/src/int/i15_decred.c
	submodules/bearssl/src/int/i15_encode.c
	submodules/bearssl/src/int/i15_fmont.c
	submodules/bearssl/src/int/i15_iszero.c
	submodules/bearssl/src/int/i15_moddiv.c
	submodules/bearssl/src/int/i15_modpow.c
	submodules/bearssl/src/int/i15_modpow2.c
	submodules/bearssl/src/int/i15_montmul.c
	submodules/bearssl/src/int/i15_mulacc.c
	submodules/bearssl/src/int/i15_muladd.c
	submodules/bearssl/src/int/i15_ninv15.c
	submodules/bearssl/src/int/i15_reduce.c
	submodules/bearssl/src/int/i15_rshift.c
	submodules/bearssl/src/int/i15_sub.c
	submodules/bearssl/src/int/i15_tmont.c
	submodules/bearssl/src/int/i31_add.c
	submodules/bearssl/src/int/i31_bitlen.c
	submodules/bearssl/src/int/i31_decmod.c
	submodules/bearssl/src/int/i31_decode.c
	submodules/bearssl/src/int/i31_decred.c
	submodules/bearssl/src/int/i31_encode.c
	submodules/bearssl/src/int/i31_fmont.c
	submodules/bearssl/src/int/i31_iszero.c
	submodules/bearssl/src/int/i31_moddiv.c
	submodules/bearssl/src/int/i31_modpow.c
	submodules/bearssl/src/int/i31_modpow2.c
	submodules/bearssl/src/int/i31_montmul.c
	submodules/bearssl/src/int/i31_mulacc.c
	submodules/bearssl/src/int/i31_muladd.c
	submodules/bearssl/src/int/i31_ninv31.c
	submodules/bearssl/src/int/i31_reduce.c
	submodules/bearssl/src/int/i31_rshift.c
	submodules/bearssl/src/int/i31_sub.c
	submodules/bearssl/src/int/i31_tmont.c
	submodules/bearssl/src/int/i32_add.c
	submodules/bearssl/src/int/i32_bitlen.c
	submodules/bearssl/src/int/i32_decmod.c
	submodules/bearssl/src/int/i32_decode.c
	submodules/bearssl/src/int/i32_decred.c
	submodules/bearssl/src/int/i32_div32.c
	submodules/bearssl/src/int/i32_encode.c
	submodules/bearssl/src/int/i32_fmont.c
	submodules/bearssl/src/int/i32_iszero.c
	submodules/bearssl/src/int/i32_modpow.c
	submodules/bearssl/src/int/i32_montmul.c
	submodules/bearssl/src/int/i32_mulacc.c
	submodules/bearssl/src/int/i32_muladd.c
	submodules/bearssl/src/int/i32_ninv32.c
	submodules/bearssl/src/int/i32_reduce.c
	submodules/bearssl/src/int/i32_sub.c
	submodules/bearssl/src/int/i32_tmont.c
	submodules/bearssl/src/int/i62_modpow2.c
	submodules/bearssl/src/kdf/hkdf.c
	submodules/bearssl/src/kdf/shake.c
	submodules/bearssl/src/mac/hmac.c
	submodules/bearssl/src/mac/hmac_ct.c
	submodules/bearssl/src/rand/aesctr_drbg.c
	submodules/bearssl/src/rand/hmac_drbg.c
	submodules/bearssl/src/rand/sysrng.c
	submodules/bearssl/src/rsa/rsa_default_keygen.c
	submodules/bearssl/src/rsa/rsa_default_modulus.c
	submodules/bearssl/src/rsa/rsa_default_oaep_decrypt.c
	submodules/bearssl/src/rsa/rsa_default_oaep_encrypt.c
	submodules/bearssl/src/rsa/rsa_default_pkcs1_sign.c
	submodules/bearssl/src/rsa/rsa_default_pkcs1_vrfy.c
	submodules/bearssl/src/rsa/rsa_default_priv.c
	submodules/bearssl/src/rsa/rsa_default_privexp.c
	submodules/bearssl/src/rsa/rsa_default_pss_sign.c
	submodules/bearssl/src/rsa/rsa_default_pss_vrfy.c
	submodules/bearssl/src/rsa/rsa_default_pub.c
	submodules/bearssl/src/rsa/rsa_default_pubexp.c
	submodules/bearssl/src/rsa/rsa_i15_keygen.c
	submodules/bearssl/src/rsa/rsa_i15_modulus.c
	submodules/bearssl/src/rsa/rsa_i15_oaep_decrypt.c
	submodules/bearssl/src/rsa/rsa_i15_oaep_encrypt.c
	submodules/bearssl/src/rsa/rsa_i15_pkcs1_sign.c
	submodules/bearssl/src/rsa/rsa_i15_pkcs1_vrfy.c
	submodules/bearssl/src/rsa/rsa_i15_priv.c
	submodules/bearssl/src/rsa/rsa_i15_privexp.c
	submodules/bearssl/src/rsa/rsa_i15_pss_sign.c
	submodules/bearssl/src/rsa/rsa_i15_pss_vrfy.c
	submodules/bearssl/src/rsa/rsa_i15_pub.c
	submodules/bearssl/src/rsa/rsa_i15_pubexp.c
	submodules/bearssl/src/rsa/rsa_i31_keygen.c
	submodules/bearssl/src/rsa/rsa_i31_keygen_inner.c
	submodules/bearssl/src/rsa/rsa_i31_modulus.c
	submodules/bearssl/src/rsa/rsa_i31_oaep_decrypt.c
	submodules/bearssl/src/rsa/rsa_i31_oaep_encrypt.c
	submodules/bearssl/src/rsa/rsa_i31_pkcs1_sign.c
	submodules/bearssl/src/rsa/rsa_i31_pkcs1_vrfy.c
	submodules/bearssl/src/rsa/rsa_i31_priv.c
	submodules/bearssl/src/rsa/rsa_i31_privexp.c
	submodules/bearssl/src/rsa/rsa_i31_pss_sign.c
	submodules/bearssl/src/rsa/rsa_i31_pss_vrfy.c
	submodules/bearssl/src/rsa/rsa_i31_pub.c
	submodules/bearssl/src/rsa/rsa_i31_pubexp.c
	submodules/bearssl/src/rsa/rsa_i32_oaep_decrypt.c
	submodules/bearssl/src/rsa/rsa_i32_oaep_encrypt.c
	submodules/bearssl/src/rsa/rsa_i32_pkcs1_sign.c
	submodules/bearssl/src/rsa/rsa_i32_pkcs1_vrfy.c
	submodules/bearssl/src/rsa/rsa_i32_priv.c
	submodules/bearssl/src/rsa/rsa_i32_pss_sign.c
	submodules/bearssl/src/rsa/rsa_i32_pss_vrfy.c
	submodules/bearssl/src/rsa/rsa_i32_pub.c
	submodules/bearssl/src/rsa/rsa_i62_keygen.c
	submodules/bearssl/src/rsa/rsa_i62_oaep_decrypt.c
	submodules/bearssl/src/rsa/rsa_i62_oaep_encrypt.c
	submodules/bearssl/src/rsa/rsa_i62_pkcs1_sign.c
	submodules/bearssl/src/rsa/rsa_i62_pkcs1_vrfy.c
	submodules/bearssl/src/rsa/rsa_i62_priv.c
	submodules/bearssl/src/rsa/rsa_i62_pss_sign.c
	submodules/bearssl/src/rsa/rsa_i62_pss_vrfy.c
	submodules/bearssl/src/rsa/rsa_i62_pub.c
	submodules/bearssl/src/rsa/rsa_oaep_pad.c
	submodules/bearssl/src/rsa/rsa_oaep_unpad.c
	submodules/bearssl/src/rsa/rsa_pkcs1_sig_pad.c
	submodules/bearssl/src/rsa/rsa_pkcs1_sig_unpad.c
	submodules/bearssl/src/rsa/rsa_pss_sig_pad.c
	submodules/bearssl/src/rsa/rsa_pss_sig_unpad.c
	submodules/bearssl/src/rsa/rsa_ssl_decrypt.c
	submodules/bearssl/src/ssl/prf.c
	submodules/bearssl/src/ssl/prf_md5sha1.c
	submodules/bearssl/src/ssl/prf_sha256.c
	submodules/bearssl/src/ssl/prf_sha384.c
	submodules/bearssl/src/ssl/ssl_ccert_single_ec.c
	submodules/bearssl/src/ssl/ssl_ccert_single_rsa.c
	submodules/bearssl/src/ssl/ssl_client.c
	submodules/bearssl/src/ssl/ssl_client_default_rsapub.c
	submodules/bearssl/src/ssl/ssl_client_full.c
	submodules/bearssl/src/ssl/ssl_engine.c
	submodules/bearssl/src/ssl/ssl_engine_default_aescbc.c
	submodules/bearssl/src/ssl/ssl_engine_default_aesccm.c
	submodules/bearssl/src/ssl/ssl_engine_default_aesgcm.c
	submodules/bearssl/src/ssl/ssl_engine_default_chapol.c
	submodules/bearssl/src/ssl/ssl_engine_default_descbc.c
	submodules/bearssl/src/ssl/ssl_engine_default_ec.c
	submodules/bearssl/src/ssl/ssl_engine_default_ecdsa.c
	submodules/bearssl/src/ssl/ssl_engine_default_rsavrfy.c
	submodules/bearssl/src/ssl/ssl_hashes.c
	submodules/bearssl/src/ssl/ssl_hs_client.c
	submodules/bearssl/src/ssl/ssl_hs_server.c
	submodules/bearssl/src/ssl/ssl_io.c
	submodules/bearssl/src/ssl/ssl_keyexport.c
	submodules/bearssl/src/ssl/ssl_lru.c
	submodules/bearssl/src/ssl/ssl_rec_cbc.c
	submodules/bearssl/src/ssl/ssl_rec_ccm.c
	submodules/bearssl/src/ssl/ssl_rec_chapol.c
	submodules/bearssl/src/ssl/ssl_rec_gcm.c
	submodules/bearssl/src/ssl/ssl_scert_single_ec.c
	submodules/bearssl/src/ssl/ssl_scert_single_rsa.c
	submodules/bearssl/src/ssl/ssl_server.c
	submodules/bearssl/src/ssl/ssl_server_full_ec.c
	submodules/bearssl/src/ssl/ssl_server_full_rsa.c
	submodules/bearssl/src/ssl/ssl_server_mine2c.c
	submodules/bearssl/src/ssl/ssl_server_mine2g.c
	submodules/bearssl/src/ssl/ssl_server_minf2c.c
	submodules/bearssl/src/ssl/ssl_server_minf2g.c
	submodules/bearssl/src/ssl/ssl_server_minr2g.c
	submodules/bearssl/src/ssl/ssl_server_minu2g.c
	submodules/bearssl/src/ssl/ssl_server_minv2g.c
	submodules/bearssl/src/symcipher/aes_big_cbcdec.c
	submodules/bearssl/src/symcipher/aes_big_cbcenc.c
	submodules/bearssl/src/symcipher/aes_big_ctr.c
	submodules/bearssl/src/symcipher/aes_big_ctrcbc.c
	submodules/bearssl/src/symcipher/aes_big_dec.c
	submodules/bearssl/src/symcipher/aes_big_enc.c
	submodules/bearssl/src/symcipher/aes_common.c
	submodules/bearssl/src/symcipher/aes_ct.c
	submodules/bearssl/src/symcipher/aes_ct64.c
	submodules/bearssl/src/symcipher/aes_ct64_cbcdec.c
	submodules/bearssl/src/symcipher/aes_ct64_cbcenc.c
	submodules/bearssl/src/symcipher/aes_ct64_ctr.c
	submodules/bearssl/src/symcipher/aes_ct64_ctrcbc.c
	submodules/bearssl/src/symcipher/aes_ct64_dec.c
	submodules/bearssl/src/symcipher/aes_ct64_enc.c
	submodules/bearssl/src/symcipher/aes_ct_cbcdec.c
	submodules/bearssl/src/symcipher/aes_ct_cbcenc.c
	submodules/bearssl/src/symcipher/aes_ct_ctr.c
	submodules/bearssl/src/symcipher/aes_ct_ctrcbc.c
	submodules/bearssl/src/symcipher/aes_ct_dec.c
	submodules/bearssl/src/symcipher/aes_ct_enc.c
	submodules/bearssl/src/symcipher/aes_pwr8.c
	submodules/bearssl/src/symcipher/aes_pwr8_cbcdec.c
	submodules/bearssl/src/symcipher/aes_pwr8_cbcenc.c
	submodules/bearssl/src/symcipher/aes_pwr8_ctr.c
	submodules/bearssl/src/symcipher/aes_pwr8_ctrcbc.c
	submodules/bearssl/src/symcipher/aes_small_cbcdec.c
	submodules/bearssl/src/symcipher/aes_small_cbcenc.c
	submodules/bearssl/src/symcipher/aes_small_ctr.c
	submodules/bearssl/src/symcipher/aes_small_ctrcbc.c
	submodules/bearssl/src/symcipher/aes_small_dec.c
	submodules/bearssl/src/symcipher/aes_small_enc.c
	submodules/bearssl/src/symcipher/aes_x86ni.c
	submodules/bearssl/src/symcipher/aes_x86ni_cbcdec.c
	submodules/bearssl/src/symcipher/aes_x86ni_cbcenc.c
	submodules/bearssl/src/symcipher/aes_x86ni_ctr.c
	submodules/bearssl/src/symcipher/aes_x86ni_ctrcbc.c
	submodules/bearssl/src/symcipher/chacha20_ct.c
	submodules/bearssl/src/symcipher/chacha20_sse2.c
	submodules/bearssl/src/symcipher/des_ct.c
	submodules/bearssl/src/symcipher/des_ct_cbcdec.c
	submodules/bearssl/src/symcipher/des_ct_cbcenc.c
	submodules/bearssl/src/symcipher/des_support.c
	submodules/bearssl/src/symcipher/des_tab.c
	submodules/bearssl/src/symcipher/des_tab_cbcdec.c
	submodules/bearssl/src/symcipher/des_tab_cbcenc.c
	submodules/bearssl/src/symcipher/poly1305_ctmul.c
	submodules/bearssl/src/symcipher/poly1305_ctmul32.c
	submodules/bearssl/src/symcipher/poly1305_ctmulq.c
	submodules/bearssl/src/symcipher/poly1305_i15.c
	submodules/bearssl/src/x509/asn1enc.c
	submodules/bearssl/src/x509/encode_ec_pk8der.c
	submodules/bearssl/src/x509/encode_ec_rawder.c
	submodules/bearssl/src/x509/encode_rsa_pk8der.c
	submodules/bearssl/src/x509/encode_rsa_rawder.c
	submodules/bearssl/src/x509/skey_decoder.c
	submodules/bearssl/src/x509/x509_decoder.c
	submodules/bearssl/src/x509/x509_knownkey.c
	submodules/bearssl/src/x509/x509_minimal.c
	submodules/bearssl/src/x509/x509_minimal_full.c
)

add_subdirectory(submodules/jansson EXCLUDE_FROM_ALL)
add_subdirectory(submodules/pcre2 EXCLUDE_FROM_ALL)

add_executable(
	unalix
	src/clean_url.c
	src/errors.c
	src/query.c
	src/regex.c
	src/ruleset.c
	src/uri.c
	src/utils.c
	src/http.c
	src/unshort_url.c
	src/callbacks.c
	src/address.c
	src/connection.c
)

foreach(target pcre2-8-shared jansson)
	add_custom_command(
		TARGET unalix POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:${target}>
		$<TARGET_FILE_DIR:unalix>
	)
endforeach()

# Test suite
if (UNALIX_BUILD_TESTING)
	add_executable(test_uri test/test_uri.c)
	target_link_libraries(test_uri unalix)
	add_test(NAME test_uri COMMAND test_uri)
	
	add_executable(test_query test/test_query.c)
	target_link_libraries(test_query unalix)
	add_test(NAME test_query COMMAND test_query)
	
	add_executable(test_clean_url test/test_clean_url.c)
	target_link_libraries(test_clean_url unalix)
	add_test(NAME test_clean_url COMMAND test_clean_url)
	
	add_executable(test_http test/test_http.c)
	target_link_libraries(test_http unalix)
	add_test(NAME test_http COMMAND test_http)
	
	enable_testing()
endif()

target_link_libraries(
	unalix
	jansson
	bearssl
	pcre2-8-shared
)

install(
	TARGETS unalix
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
)

install(
	TARGETS pcre2-8-shared
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
)

install(
	TARGETS jansson
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
)

if (UNIX)
	set_target_properties(
		unalix PROPERTIES INSTALL_RPATH "$ORIGIN/../lib"
	)
endif()