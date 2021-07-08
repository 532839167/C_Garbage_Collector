/*
 * =====================================================================================
 *    Filename:  mld.h
 *    Description:  This file defines the data structures used for MLD tool
 * =====================================================================================
 */

#ifndef __MLD__
#include <assert.h>
#include <string.h>

/* Structure Record Database Definition */

#define MAX_STRUCTURE_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128

/*Enumeration for data types for fields*/
typedef enum {
    UINT8,
    UINT32,
    INT32,
    CHAR,
    OBJ_PTR,
    VOID_PTR, /*New Data type added to identify void * pointers*/
    FLOAT,
    DOUBLE,
    OBJ_STRUCT
} data_type_t;

typedef enum{
    MLD_FALSE,
    MLD_TRUE
} mld_boolean_t;

/* Macro to compute the offset of a field */
#define OFFSETOF(struct_name, fld_name)     \
    (unsigned long)&(((struct_name *)0)->fld_name)

/* Macro to compute the size of a field */
#define FIELD_SIZE(struct_name, fld_name)   \
    sizeof(((struct_name *)0)->fld_name)


typedef struct _struct_db_rec_ struct_db_rec_t;


/*Structure to store the information of one field of a C structure*/
typedef struct _field_info_{
    char fname [MAX_FIELD_NAME_SIZE];   /*Name of the field*/
    data_type_t dtype;                  /*Data type of the field*/
    unsigned int size;                  /*Size of the field*/
    unsigned int offset;                /*Offset of the field*/
    // Below field is meaningful only if dtype = OBJ_PTR, Or OBJ_STRUCT
    char nested_str_name[MAX_STRUCTURE_NAME_SIZE];
} field_info_t;

/* Structure to store the information of one C structure which could have 'n_fields' fields */
struct _struct_db_rec_{
    struct_db_rec_t *next;  /*Pointer to the next structure in the linked list*/
    char struct_name [MAX_STRUCTURE_NAME_SIZE];  // key
    unsigned int ds_size;   /*Size of the structure*/
    unsigned int n_fields;  /*No of fields in the structure*/
    field_info_t *fields;   /*pointer to the array of fields*/
};

/*Finally the head of the linked list representing the structure record list */
typedef struct _struct_db_{
    struct_db_rec_t *head;
    unsigned int count;
} struct_db_t;

/* Structure Record Database Definition Ends */




/* Printing functions*/
void
print_structure_rec (struct_db_rec_t *struct_rec);

void
print_structure_db(struct_db_t *struct_db);

/* Fn to add the structure record in a structure database */

int /*return 0 on success, -1 on failure for some reason*/
add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct_rec);

/*Structure Registration helping APIs*/

#define FIELD_INFO(struct_name, fld_name, dtype, nested_struct_name)    \
   {#fld_name, dtype, FIELD_SIZE(struct_name, fld_name),                \
        OFFSETOF(struct_name, fld_name), #nested_struct_name} 

#define REG_STRUCT(struct_db, st_name, fields_arr)                    \
    do{                                                               \
        struct_db_rec_t *rec = calloc(1, sizeof(struct_db_rec_t));    \
        strncpy(rec->struct_name, #st_name, MAX_STRUCTURE_NAME_SIZE); \
        rec->ds_size = sizeof(st_name);                              \
        rec->n_fields = sizeof(fields_arr)/sizeof(field_info_t);     \
        rec->fields = fields_arr;                                    \
        if(add_structure_to_struct_db(struct_db, rec)){              \
            assert(0);                                               \
        }                                                            \
    }while(0);

/*Structure Data base Definition Ends*/






/*Object Database structure definitions Starts here*/

typedef struct _object_db_rec_ object_db_rec_t;

/* Structure to store the information of one object record */
struct _object_db_rec_{
    object_db_rec_t *next; // pointer to the next object record in the linked-list
    void *ptr; // pointer to the object the application just malloc'd
    unsigned int units; // number of units of the malloc'd object
    struct_db_rec_t *struct_rec; // pointer to the structure database record
    mld_boolean_t is_visited; /* For Graph traversal */
    mld_boolean_t is_root;    /* Is this object the Root object */
};

typedef struct _object_db_{
    struct_db_t *struct_db; // pointer to the struct_db
    object_db_rec_t *head; // pointer to the 1st object record
    unsigned int count; // how many records in total
} object_db_t;


/*Dumping functions*/
void
print_object_rec(object_db_rec_t *obj_rec, int i);

void
print_object_db(object_db_t *object_db);

/*API to malloc the object*/
void*
xcalloc(object_db_t *object_db, char *struct_name, int units);

/*APIs to register root objects*/
void mld_register_root_object (object_db_t *object_db, 
                               void *objptr, 
                               char *struct_name, 
                               unsigned int units);

void
set_mld_object_as_global_root(object_db_t *object_db, void *obj_ptr);


/*APIs for MLD Algorithm*/
void
run_mld_algorithm(object_db_t *object_db);

void
report_leaked_objects(object_db_t *object_db);

void
mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr);

void 
mld_init_primitive_data_types_support(struct_db_t *struct_db);

#endif /* __MLD__ */