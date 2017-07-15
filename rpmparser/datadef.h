#ifndef _DAA_H_
#define _DAA_H_
struct rpmlead {
    unsigned char magic[4];
    unsigned char major;
    unsigned char minor;
    short type;
    short archnum;
    char name[66];
    short osnum;
    short signature_type;       /*!< Signature header type (RPMSIG_HEADERSIG) */
    char reserved[16];      /*!< Pad to 96 bytes -- 8 byte aligned! */
};
#endif
