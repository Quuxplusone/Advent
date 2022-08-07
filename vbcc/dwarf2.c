/*  $VER: vbcc (dwarf2.c) $Revision: 1.5 $     */

enum dwarf_tag
{
  DW_TAG_padding = 0x00,
  DW_TAG_array_type = 0x01,
  DW_TAG_class_type = 0x02,
  DW_TAG_entry_point = 0x03,
  DW_TAG_enumeration_type = 0x04,
  DW_TAG_formal_parameter = 0x05,
  DW_TAG_imported_declaration = 0x08,
  DW_TAG_label = 0x0a,
  DW_TAG_lexical_block = 0x0b,
  DW_TAG_member = 0x0d,
  DW_TAG_pointer_type = 0x0f,
  DW_TAG_reference_type = 0x10,
  DW_TAG_compile_unit = 0x11,
  DW_TAG_string_type = 0x12,
  DW_TAG_structure_type = 0x13,
  DW_TAG_subroutine_type = 0x15,
  DW_TAG_typedef = 0x16,
  DW_TAG_union_type = 0x17,
  DW_TAG_unspecified_parameters = 0x18,
  DW_TAG_variant = 0x19,
  DW_TAG_common_block = 0x1a,
  DW_TAG_common_inclusion = 0x1b,
  DW_TAG_inheritance = 0x1c,
  DW_TAG_inlined_subroutine = 0x1d,
  DW_TAG_module = 0x1e,
  DW_TAG_ptr_to_member_type = 0x1f,
  DW_TAG_set_type = 0x20,
  DW_TAG_subrange_type = 0x21,
  DW_TAG_with_stmt = 0x22,
  DW_TAG_access_declaration = 0x23,
  DW_TAG_base_type = 0x24,
  DW_TAG_catch_block = 0x25,
  DW_TAG_const_type = 0x26,
  DW_TAG_constant = 0x27,
  DW_TAG_enumerator = 0x28,
  DW_TAG_file_type = 0x29,
  DW_TAG_friend = 0x2a,
  DW_TAG_namelist = 0x2b,
  DW_TAG_namelist_item = 0x2c,
  DW_TAG_packed_type = 0x2d,
  DW_TAG_subprogram = 0x2e,
  DW_TAG_template_type_param = 0x2f,
  DW_TAG_template_value_param = 0x30,
  DW_TAG_thrown_type = 0x31,
  DW_TAG_try_block = 0x32,
  DW_TAG_variant_part = 0x33,
  DW_TAG_variable = 0x34,
  DW_TAG_volatile_type = 0x35,
  /* SGI/MIPS Extensions */
  DW_TAG_MIPS_loop = 0x4081,
  /* GNU extensions */
  DW_TAG_format_label = 0x4101,
  DW_TAG_function_template = 0x4102,
  DW_TAG_class_template = 0x4103
};

enum dwarf_children
{
  DW_children_no = 0,
  DW_children_yes = 1
};

enum dwarf_form
{
  DW_FORM_addr = 0x01,
  DW_FORM_block2 = 0x03,
  DW_FORM_block4 = 0x04,
  DW_FORM_data2 = 0x05,
  DW_FORM_data4 = 0x06,
  DW_FORM_data8 = 0x07,
  DW_FORM_string = 0x08,
  DW_FORM_block = 0x09,
  DW_FORM_block1 = 0x0a,
  DW_FORM_data1 = 0x0b,
  DW_FORM_flag = 0x0c,
  DW_FORM_sdata = 0x0d,
  DW_FORM_strp = 0x0e,
  DW_FORM_udata = 0x0f,
  DW_FORM_ref_addr = 0x10,
  DW_FORM_ref1 = 0x11,
  DW_FORM_ref2 = 0x12,
  DW_FORM_ref4 = 0x13,
  DW_FORM_ref8 = 0x14,
  DW_FORM_ref_udata = 0x15,
  DW_FORM_indirect = 0x16
};

enum dwarf_attribute
{
  DW_AT_sibling = 0x01,
  DW_AT_location = 0x02,
  DW_AT_name = 0x03,
  DW_AT_ordering = 0x09,
  DW_AT_subscr_data = 0x0a,
  DW_AT_byte_size = 0x0b,
  DW_AT_bit_offset = 0x0c,
  DW_AT_bit_size = 0x0d,
  DW_AT_element_list = 0x0f,
  DW_AT_stmt_list = 0x10,
  DW_AT_low_pc = 0x11,
  DW_AT_high_pc = 0x12,
  DW_AT_language = 0x13,
  DW_AT_member = 0x14,
  DW_AT_discr = 0x15,
  DW_AT_discr_value = 0x16,
  DW_AT_visibility = 0x17,
  DW_AT_import = 0x18,
  DW_AT_string_length = 0x19,
  DW_AT_common_reference = 0x1a,
  DW_AT_comp_dir = 0x1b,
  DW_AT_const_value = 0x1c,
  DW_AT_containing_type = 0x1d,
  DW_AT_default_value = 0x1e,
  DW_AT_inline = 0x20,
  DW_AT_is_optional = 0x21,
  DW_AT_lower_bound = 0x22,
  DW_AT_producer = 0x25,
  DW_AT_prototyped = 0x27,
  DW_AT_return_addr = 0x2a,
  DW_AT_start_scope = 0x2c,
  DW_AT_stride_size = 0x2e,
  DW_AT_upper_bound = 0x2f,
  DW_AT_abstract_origin = 0x31,
  DW_AT_accessibility = 0x32,
  DW_AT_address_class = 0x33,
  DW_AT_artificial = 0x34,
  DW_AT_base_types = 0x35,
  DW_AT_calling_convention = 0x36,
  DW_AT_count = 0x37,
  DW_AT_data_member_location = 0x38,
  DW_AT_decl_column = 0x39,
  DW_AT_decl_file = 0x3a,
  DW_AT_decl_line = 0x3b,
  DW_AT_declaration = 0x3c,
  DW_AT_discr_list = 0x3d,
  DW_AT_encoding = 0x3e,
  DW_AT_external = 0x3f,
  DW_AT_frame_base = 0x40,
  DW_AT_friend = 0x41,
  DW_AT_identifier_case = 0x42,
  DW_AT_macro_info = 0x43,
  DW_AT_namelist_items = 0x44,
  DW_AT_priority = 0x45,
  DW_AT_segment = 0x46,
  DW_AT_specification = 0x47,
  DW_AT_static_link = 0x48,
  DW_AT_type = 0x49,
  DW_AT_use_location = 0x4a,
  DW_AT_variable_parameter = 0x4b,
  DW_AT_virtuality = 0x4c,
  DW_AT_vtable_elem_location = 0x4d,
  DW_AT_MIPS_fde = 0x2001,
  DW_AT_MIPS_loop_begin = 0x2002,
  DW_AT_MIPS_tail_loop_begin = 0x2003,
  DW_AT_MIPS_epilog_begin = 0x2004,
  DW_AT_MIPS_loop_unroll_factor = 0x2005,
  DW_AT_MIPS_software_pipeline_depth = 0x2006,
  DW_AT_MIPS_linkage_name = 0x2007,
  DW_AT_MIPS_stride = 0x2008,
  DW_AT_MIPS_abstract_name = 0x2009,
  DW_AT_MIPS_clone_origin = 0x200a,
  DW_AT_MIPS_has_inlines = 0x200b,
  /* GNU */
  DW_AT_sf_names = 0x2101,
  DW_AT_src_info = 0x2102,
  DW_AT_mac_info = 0x2103,
  DW_AT_src_coords = 0x2104,
  DW_AT_body_begin = 0x2105,
  DW_AT_body_end = 0x2106
};

enum dwarf_location_atom
{
  DW_OP_addr = 0x03,
  DW_OP_deref = 0x06,
  DW_OP_const1u = 0x08,
  DW_OP_const1s = 0x09,
  DW_OP_const2u = 0x0a,
  DW_OP_const2s = 0x0b,
  DW_OP_const4u = 0x0c,
  DW_OP_const4s = 0x0d,
  DW_OP_const8u = 0x0e,
  DW_OP_const8s = 0x0f,
  DW_OP_constu = 0x10,
  DW_OP_consts = 0x11,
  DW_OP_dup = 0x12,
  DW_OP_drop = 0x13,
  DW_OP_over = 0x14,
  DW_OP_pick = 0x15,
  DW_OP_swap = 0x16,
  DW_OP_rot = 0x17,
  DW_OP_xderef = 0x18,
  DW_OP_abs = 0x19,
  DW_OP_and = 0x1a,
  DW_OP_div = 0x1b,
  DW_OP_minus = 0x1c,
  DW_OP_mod = 0x1d,
  DW_OP_mul = 0x1e,
  DW_OP_neg = 0x1f,
  DW_OP_not = 0x20,
  DW_OP_or = 0x21,
  DW_OP_plus = 0x22,
  DW_OP_plus_uconst = 0x23,
  DW_OP_shl = 0x24,
  DW_OP_shr = 0x25,
  DW_OP_shra = 0x26,
  DW_OP_xor = 0x27,
  DW_OP_bra = 0x28,
  DW_OP_eq = 0x29,
  DW_OP_ge = 0x2a,
  DW_OP_gt = 0x2b,
  DW_OP_le = 0x2c,
  DW_OP_lt = 0x2d,
  DW_OP_ne = 0x2e,
  DW_OP_skip = 0x2f,
  DW_OP_lit0 = 0x30,
  DW_OP_lit1 = 0x31,
  DW_OP_lit2 = 0x32,
  DW_OP_lit3 = 0x33,
  DW_OP_lit4 = 0x34,
  DW_OP_lit5 = 0x35,
  DW_OP_lit6 = 0x36,
  DW_OP_lit7 = 0x37,
  DW_OP_lit8 = 0x38,
  DW_OP_lit9 = 0x39,
  DW_OP_lit10 = 0x3a,
  DW_OP_lit11 = 0x3b,
  DW_OP_lit12 = 0x3c,
  DW_OP_lit13 = 0x3d,
  DW_OP_lit14 = 0x3e,
  DW_OP_lit15 = 0x3f,
  DW_OP_lit16 = 0x40,
  DW_OP_lit17 = 0x41,
  DW_OP_lit18 = 0x42,
  DW_OP_lit19 = 0x43,
  DW_OP_lit20 = 0x44,
  DW_OP_lit21 = 0x45,
  DW_OP_lit22 = 0x46,
  DW_OP_lit23 = 0x47,
  DW_OP_lit24 = 0x48,
  DW_OP_lit25 = 0x49,
  DW_OP_lit26 = 0x4a,
  DW_OP_lit27 = 0x4b,
  DW_OP_lit28 = 0x4c,
  DW_OP_lit29 = 0x4d,
  DW_OP_lit30 = 0x4e,
  DW_OP_lit31 = 0x4f,
  DW_OP_reg0 = 0x50,
  DW_OP_reg1 = 0x51,
  DW_OP_reg2 = 0x52,
  DW_OP_reg3 = 0x53,
  DW_OP_reg4 = 0x54,
  DW_OP_reg5 = 0x55,
  DW_OP_reg6 = 0x56,
  DW_OP_reg7 = 0x57,
  DW_OP_reg8 = 0x58,
  DW_OP_reg9 = 0x59,
  DW_OP_reg10 = 0x5a,
  DW_OP_reg11 = 0x5b,
  DW_OP_reg12 = 0x5c,
  DW_OP_reg13 = 0x5d,
  DW_OP_reg14 = 0x5e,
  DW_OP_reg15 = 0x5f,
  DW_OP_reg16 = 0x60,
  DW_OP_reg17 = 0x61,
  DW_OP_reg18 = 0x62,
  DW_OP_reg19 = 0x63,
  DW_OP_reg20 = 0x64,
  DW_OP_reg21 = 0x65,
  DW_OP_reg22 = 0x66,
  DW_OP_reg23 = 0x67,
  DW_OP_reg24 = 0x68,
  DW_OP_reg25 = 0x69,
  DW_OP_reg26 = 0x6a,
  DW_OP_reg27 = 0x6b,
  DW_OP_reg28 = 0x6c,
  DW_OP_reg29 = 0x6d,
  DW_OP_reg30 = 0x6e,
  DW_OP_reg31 = 0x6f,
  DW_OP_breg0 = 0x70,
  DW_OP_breg1 = 0x71,
  DW_OP_breg2 = 0x72,
  DW_OP_breg3 = 0x73,
  DW_OP_breg4 = 0x74,
  DW_OP_breg5 = 0x75,
  DW_OP_breg6 = 0x76,
  DW_OP_breg7 = 0x77,
  DW_OP_breg8 = 0x78,
  DW_OP_breg9 = 0x79,
  DW_OP_breg10 = 0x7a,
  DW_OP_breg11 = 0x7b,
  DW_OP_breg12 = 0x7c,
  DW_OP_breg13 = 0x7d,
  DW_OP_breg14 = 0x7e,
  DW_OP_breg15 = 0x7f,
  DW_OP_breg16 = 0x80,
  DW_OP_breg17 = 0x81,
  DW_OP_breg18 = 0x82,
  DW_OP_breg19 = 0x83,
  DW_OP_breg20 = 0x84,
  DW_OP_breg21 = 0x85,
  DW_OP_breg22 = 0x86,
  DW_OP_breg23 = 0x87,
  DW_OP_breg24 = 0x88,
  DW_OP_breg25 = 0x89,
  DW_OP_breg26 = 0x8a,
  DW_OP_breg27 = 0x8b,
  DW_OP_breg28 = 0x8c,
  DW_OP_breg29 = 0x8d,
  DW_OP_breg30 = 0x8e,
  DW_OP_breg31 = 0x8f,
  DW_OP_regx = 0x90,
  DW_OP_fbreg = 0x91,
  DW_OP_bregx = 0x92,
  DW_OP_piece = 0x93,
  DW_OP_deref_size = 0x94,
  DW_OP_xderef_size = 0x95,
  DW_OP_nop = 0x96
};

enum dwarf_type
{
  DW_ATE_void = 0x0,
  DW_ATE_address = 0x1,
  DW_ATE_boolean = 0x2,
  DW_ATE_complex_float = 0x3,
  DW_ATE_float = 0x4,
  DW_ATE_signed = 0x5,
  DW_ATE_signed_char = 0x6,
  DW_ATE_unsigned = 0x7,
  DW_ATE_unsigned_char = 0x8
};

enum dwarf_array_dim_ordering
{
  DW_ORD_row_major = 0,
  DW_ORD_col_major = 1
};

enum dwarf_access_attribute
{
  DW_ACCESS_public = 1,
  DW_ACCESS_protected = 2,
  DW_ACCESS_private = 3
};

enum dwarf_visibility_attribute
{
  DW_VIS_local = 1,
  DW_VIS_exported = 2,
  DW_VIS_qualified = 3
};

enum dwarf_virtuality_attribute
{
  DW_VIRTUALITY_none = 0,
  DW_VIRTUALITY_virtual = 1,
  DW_VIRTUALITY_pure_virtual = 2
};

enum dwarf_id_case
{
  DW_ID_case_sensitive = 0,
  DW_ID_up_case = 1,
  DW_ID_down_case = 2,
  DW_ID_case_insensitive = 3
};

enum dwarf_calling_convention
{
  DW_CC_normal = 0x1,
  DW_CC_program = 0x2,
  DW_CC_nocall = 0x3
};

enum dwarf_inline_attribute
{
  DW_INL_not_inlined = 0,
  DW_INL_inlined = 1,
  DW_INL_declared_not_inlined = 2,
  DW_INL_declared_inlined = 3
};

enum dwarf_descrim_list
{
  DW_DSC_label = 0,
  DW_DSC_range = 1
};

enum dwarf_line_number_ops
{
  DW_LNS_extended_op = 0,
  DW_LNS_copy = 1,
  DW_LNS_advance_pc = 2,
  DW_LNS_advance_line = 3,
  DW_LNS_set_file = 4,
  DW_LNS_set_column = 5,
  DW_LNS_negate_stmt = 6,
  DW_LNS_set_basic_block = 7,
  DW_LNS_const_add_pc = 8,
  DW_LNS_fixed_advance_pc = 9
};

enum dwarf_line_number_x_ops
{
  DW_LNE_end_sequence = 1,
  DW_LNE_set_address = 2,
  DW_LNE_define_file = 3
};

enum dwarf_call_frame_info
{
  DW_CFA_advance_loc = 0x40,
  DW_CFA_offset = 0x80,
  DW_CFA_restore = 0xc0,
  DW_CFA_nop = 0x00,
  DW_CFA_set_loc = 0x01,
  DW_CFA_advance_loc1 = 0x02,
  DW_CFA_advance_loc2 = 0x03,
  DW_CFA_advance_loc4 = 0x04,
  DW_CFA_offset_extended = 0x05,
  DW_CFA_restore_extended = 0x06,
  DW_CFA_undefined = 0x07,
  DW_CFA_same_value = 0x08,
  DW_CFA_register = 0x09,
  DW_CFA_remember_state = 0x0a,
  DW_CFA_restore_state = 0x0b,
  DW_CFA_def_cfa = 0x0c,
  DW_CFA_def_cfa_register = 0x0d,
  DW_CFA_def_cfa_offset = 0x0e,
  DW_CFA_MIPS_advance_loc8 = 0x1d
};

#define DW_CIE_ID	  0xffffffff
#define DW_CIE_VERSION	  1

#define DW_CFA_extended   0
#define DW_CFA_low_user   0x1c
#define DW_CFA_high_user  0x3f

#define DW_CHILDREN_no		     0x00
#define DW_CHILDREN_yes		     0x01

#define DW_ADDR_none		0

enum dwarf_source_language
{
  DW_LANG_C89 = 0x0001,
  DW_LANG_C = 0x0002,
  DW_LANG_Ada83 = 0x0003,
  DW_LANG_C_plus_plus = 0x0004,
  DW_LANG_Cobol74 = 0x0005,
  DW_LANG_Cobol85 = 0x0006,
  DW_LANG_Fortran77 = 0x0007,
  DW_LANG_Fortran90 = 0x0008,
  DW_LANG_Pascal83 = 0x0009,
  DW_LANG_Modula2 = 0x000a,
  DW_LANG_Mips_Assembler = 0x8001
};


enum dwarf_macinfo_record_type
{
  DW_MACINFO_define = 1,
  DW_MACINFO_undef = 2,
  DW_MACINFO_start_file = 3,
  DW_MACINFO_end_file = 4,
  DW_MACINFO_vendor_ext = 255
};



#define HAVE_LOCATION_LISTS 0

/* provided by the code generator */
static void dwarf2_print_frame_location(FILE *,Var *);
static zmax dwarf2_fboffset(Var *);
static int dwarf2_regnumber(int);

static int abbrev_label,info_start,info_end,line_start,line_end;

static int sizeof_addr;
static char *dwarfd1,*dwarfd2,*dwarfd4,*da,*lp,*ip,*dsec;
static int addr_form;
static char **names;
static int namecount;

extern Var *merk_varf,*first_var[],*first_ext; /*FIXME: not nice */
extern struct_declaration *first_sd[];

#define COMP_UNIT 1UL
#define SUBPROGRAM 2UL
#define SUBPROGRAMVOID 3UL 
#define BASETYPE 4UL
#define POINTERTYPE 5UL
#define CONSTTYPE 6UL
#define VOLATILETYPE 7UL
#define FORMALPARAMETER 8UL
#define VARIABLE 9UL
#define ARRAYTYPE 10UL
#define ARRAYDIM 11UL
#define STRUCTTYPE 12UL
#define UNIONTYPE 13UL
#define STRUCTTAG 14UL
#define UNIONTAG 15UL
#define TYPEDEFTYPE 16UL
#define MEMBERTYPE 17UL
#define FUNCTYPE 18UL
#define VOIDFUNCTYPE 19UL
#define PARMTYPE 20UL

typedef struct dwarf2_line_info {
  struct dwarf2_line_info *next;
  char *id;
  int file,line,label;
} tdwarf2_line_info;

static tdwarf2_line_info *dwarf2_first_li,*dwarf2_last_li;

static void dwarf2_add_line(int file,int line,int label,char *id)
{
  tdwarf2_line_info *new,*p,*lp;
  new=mymalloc(sizeof(*new));
  new->file=file;
  new->line=line;
  new->label=label;
  if(id){
    new->id=mymalloc(strlen(id)+1);
    strcpy(new->id,id);
  }else{
    new->id=0;
  }
  new->next=0;
#if 0
  for(lp=p=dwarf2_first_li;p;p=p->next){
    if(p!=lp&&p->file==file&&p->line>=line){
      new->next=lp->next;
      lp->next=new;
      return;
    }
    lp=p;
  }
#endif
  if(dwarf2_last_li){
    dwarf2_last_li->next=new;
    dwarf2_last_li=new;
  }else{
    dwarf2_first_li=dwarf2_last_li=new;
  }
#if 0
  printf("linfo:\n");
  for(p=dwarf2_first_li;p;p=p->next)
    printf("li: line=%d\n",p->line);
#endif
}

static void dwarf2_setup(int sa,char *dwarfd1s,char *dwarfd2s,char *dwarfd4s,char *das,char *lps,char *ips,char *ds)
{
  sizeof_addr=sa;
  dwarfd1=dwarfd1s;
  dwarfd2=dwarfd2s;
  dwarfd4=dwarfd4s;
  da=das;
  lp=lps;
  ip=ips;
  dsec=ds;
  if(sizeof_addr==2)
    addr_form=DW_FORM_data2;
  else if(sizeof_addr==4)
    addr_form=DW_FORM_data4;
  else if(sizeof_addr==8)
    addr_form=DW_FORM_data8;
  else
    ierror(0);
}
static int dwarf2_uleb128_size(zumax value)
{
  int size=0;
  do{
    value=zumrshift(value,ul2zum(7UL));
    size++;
  }while(!zmeqto(value,ul2zum(0UL)));
  return size;
}
static void dwarf2_print_uleb128(FILE *f,zumax value)
{
  unsigned long byte;
  emit(f,"\t%s\t",dwarfd1);
  do{
    byte=zum2ul(zumand(value,ul2zum(127UL)));
    value=zumrshift(value,ul2zum(7UL));
    if(!zumeqto(value,ul2zum(0UL)))
      emit(f,"%lu,",byte|0x80);
    else
      emit(f,"%lu\n",byte);
  }while(!zumeqto(value,ul2zum(0UL)));
}
static int dwarf2_sleb128_size(zmax value)
{
  int more=1,size=0;
  long byte;
  do{
    byte=zm2l(zmand(value,l2zm(127L)));
    value=zmrshift(value,l2zm(7L)); /*FIXME*/
    if((zmeqto(value,l2zm(0L))&&!(byte&0x40))||
       (zmeqto(value,l2zm(-1L))&&(byte&0x40)))
      more=0;
    size++;
  }while(more);
  return size;
}
static void dwarf2_print_sleb128(FILE *f,zmax value)
{
  int more=1;
  long byte;
  emit(f,"\t%s\t",dwarfd1);
  do{
    byte=zm2l(zmand(value,l2zm(127L)));
    value=zmrshift(value,l2zm(7L)); /*FIXME*/
    if((zmeqto(value,l2zm(0L))&&!(byte&0x40))||
       (zmeqto(value,l2zm(-1L))&&(byte&0x40))){
      more=0;
      emit(f,"%ld\n",byte);
    }else{
      emit(f,"%ld,",byte|0x80);
    }
  }while(more);
}
static void dwarf2_print_location(FILE *f,obj *o)
{
  if(o->flags&(KONST|DREFOBJ)) ierror(0);
  if(!(o->flags&(VAR|REG))) ierror(0);
  if(o->flags&REG){
    int r=dwarf2_regnumber(o->reg);
    if(r<=31){
      emit(f,"\t%s\t%d\n",dwarfd2,1);
      emit(f,"\t%s\t%d\n",dwarfd1,DW_OP_reg0+r);
    }else{
      emit(f,"\t%s\t%d\n",dwarfd2,1+dwarf2_uleb128_size(ul2zum((unsigned long)r)));
      emit(f,"\t%s\t%d\n",dwarfd1,DW_OP_regx);
      dwarf2_print_uleb128(f,ul2zum(r));
    }
  }else{
    if(o->v->storage_class==STATIC||o->v->storage_class==EXTERN){
      if(cross_module&&!(o->v->flags&REFERENCED)){
	emit(f,"\t%s\t0\n",dwarfd1);
      }else{
	emit(f,"\t%s\t%d\n",dwarfd2,(int)(1+sizeof_addr));
	emit(f,"\t%s\t%d\n",dwarfd1,DW_OP_addr);
	if(o->v->storage_class==STATIC)
	  emit(f,"\t%s\t%s%ld\n",da,lp,zm2l(o->v->offset));
	else
	  emit(f,"\t%s\t%s%s\n",da,ip,o->v->identifier);
      }
    }else{
      zmax of=dwarf2_fboffset(o->v);
      emit(f,"\t%s\t%d\n",dwarfd2,1+dwarf2_sleb128_size(of));
      emit(f,"\t%s\t%d\n",dwarfd1,DW_OP_fbreg);
      dwarf2_print_sleb128(f,of);
    }
  }
}
static int dwarf2_file(const char *p)
{
  int i;
  if(!p) ierror(0);
  for(i=0;i<namecount;i++)
    if(!strcmp(p,names[i]))
      return i+1;
  namecount++;
  names=myrealloc(names,namecount*sizeof(*names));
  names[namecount-1]=mymalloc(strlen(p)+1);
  strcpy(names[namecount-1],p);
  return namecount;
}
static int dwarf2_type(FILE *f,type *t)
{
  int l,lo;char *p;
  if(ISPOINTER(t->flags)){
    lo=dwarf2_type(f,t->next);
    l=++label;
    emit(f,"%s%d:\n",lp,l);
    dwarf2_print_uleb128(f,ul2zum(POINTERTYPE));
    emit(f,"\t%s\t%s%d\n",da,lp,lo);
  }else if(ISINT(t->flags)||ISFLOAT(t->flags)||(t->flags&NQ)==VOID){
    l=++label;
    emit(f,"%s%d:\n",lp,l);
    dwarf2_print_uleb128(f,ul2zum(BASETYPE));    
    for(p=typname[t->flags&NQ];*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
    emit(f,"\t%s\t0\n",dwarfd1);
    if(ISFLOAT(t->flags)){
      lo=DW_ATE_float;
    }else if((t->flags&NQ)==CHAR){
      if(t->flags&UNSIGNED)
	lo=DW_ATE_unsigned_char;
      else
	lo=DW_ATE_signed_char;
    }else if((t->flags&NQ)==VOID){
      lo=DW_ATE_unsigned_char;
    }else{
      if(!ISINT(t->flags)) ierror(0);
      if(t->flags&UNSIGNED)
	lo=DW_ATE_unsigned;
      else
	lo=DW_ATE_signed;
    }
    dwarf2_print_uleb128(f,ul2zum((unsigned long)lo));
    dwarf2_print_uleb128(f,sizetab[t->flags&NQ]);
  }else if(ISARRAY(t->flags)){
    lo=dwarf2_type(f,t->next);
    l=++label;
    emit(f,"%s%d:\n",lp,l);
    dwarf2_print_uleb128(f,ul2zum(ARRAYTYPE));
    emit(f,"\t%s\t%s%d\n",da,lp,lo);
    dwarf2_print_uleb128(f,ul2zum(ARRAYDIM));
    dwarf2_print_uleb128(f,zumsub(t->size,ul2zum(1UL)));
    emit(f,"\t%s\t0\n",dwarfd1);
  }else if(ISSTRUCT(t->flags)||ISUNION(t->flags)){
    if(t->exact->label>0){
      l=t->exact->label;
    }else{
      int i,*tl;zmax offset=l2zm(0L),al;
      type *tp;
      l=++label;
      t->exact->label=l;
      tl=mymalloc(t->exact->count*sizeof(*tl));
      for(i=0;i<t->exact->count;i++)
	tl[i]=dwarf2_type(f,(*t->exact->sl)[i].styp);
      emit(f,"%s%d:\n",lp,l);
      if(t->exact->identifier){
	if(ISSTRUCT(t->flags))
	  dwarf2_print_uleb128(f,STRUCTTAG);
	else
	  dwarf2_print_uleb128(f,UNIONTAG);
	dwarf2_print_uleb128(f,szof(t));
	for(p=t->exact->identifier;*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
	emit(f,"\t%s\t0\n",dwarfd1);
      }else{
	if(ISSTRUCT(t->flags))
	  dwarf2_print_uleb128(f,STRUCTTYPE);
	else
	  dwarf2_print_uleb128(f,UNIONTYPE);
	dwarf2_print_uleb128(f,szof(t));
      }
      for(i=0;i<t->exact->count;i++){
	dwarf2_print_uleb128(f,MEMBERTYPE);
	for(p=(*t->exact->sl)[i].identifier;*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
	emit(f,"\t%s\t0\n",dwarfd1);
	emit(f,"\t%s\t%s%d\n",da,lp,tl[i]);
	dwarf2_print_uleb128(f,zumadd(dwarf2_uleb128_size(zm2zum(offset)),ul2zum(1L)));
	tp=(*t->exact->sl)[i].styp;
	al=falign(tp);
	offset=zmmult(zmdiv(zmadd(offset,zmsub(al,l2zm(1L))),al),al);
	emit(f,"\t%s\t%d\n",dwarfd1,DW_OP_plus_uconst);
	dwarf2_print_uleb128(f,zm2zum(offset));
	if(ISSTRUCT(t->flags))
	  offset=zmadd(offset,szof(tp));   	
      }
      emit(f,"\t%s\t0\n",dwarfd1);
      free(tl);
    }
  }else if(ISFUNC(t->flags)){
    int *tl,i;
    l=++label;
    tl=mymalloc(t->exact->count*sizeof(*tl));
    for(i=0;i<t->exact->count;i++){
      if(((*t->exact->sl)[i].styp->flags&NQ)!=VOID)
	tl[i]=dwarf2_type(f,(*t->exact->sl)[i].styp);
    }
    if((t->next->flags&NQ)==VOID){
      emit(f,"%s%d:\n",lp,l);
      dwarf2_print_uleb128(f,VOIDFUNCTYPE);
    }else{
      lo=dwarf2_type(f,t->next);
      emit(f,"%s%d:\n",lp,l);
      dwarf2_print_uleb128(f,FUNCTYPE);
      emit(f,"\t%s\t%s%d\n",da,lp,lo);
    }
    for(i=0;i<t->exact->count;i++){
      if(((*t->exact->sl)[i].styp->flags&NQ)!=VOID){
	dwarf2_print_uleb128(f,PARMTYPE);
	emit(f,"\t%s\t%s%d\n",da,lp,tl[i]);
      }
    }
    emit(f,"\t%s\t0\n",dwarfd1);
    free(tl);
  }else{
    printf("%d\n",t->flags);
    ierror(0);
  }
  if(t->flags&VOLATILE){
    lo=l;l=++label;
    emit(f,"%s%d:\n",lp,l);
    dwarf2_print_uleb128(f,ul2zum(VOLATILETYPE));
    emit(f,"\t%s\t%s%d\n",da,lp,lo);
  }
  if(t->flags&CONST){
    lo=l;l=++label;
    emit(f,"%s%d:\n",lp,l);
    dwarf2_print_uleb128(f,ul2zum(CONSTTYPE));
    emit(f,"\t%s\t%s%d\n",da,lp,lo);
  }
  return l;
}
static void dwarf2_var(FILE *f,Var *v)
{
  char *p;int l;
  if(!(v->flags&(DEFINED|TENTATIVE))) return;
  if(v->storage_class==STATIC&&v->nesting>0&&!(v->flags&(USEDASSOURCE|USEDASDEST))) return;

  if(ISFUNC(v->vtyp->flags)) return;
  if(*v->identifier&&v->storage_class!=TYPEDEF){
    l=dwarf2_type(f,v->vtyp);
    if((v->storage_class==AUTO||v->storage_class==REGISTER)&&(!zmleq(l2zm(0L),v->offset)||v->reg)){
      dwarf2_print_uleb128(f,ul2zum(FORMALPARAMETER));
    }else{
      dwarf2_print_uleb128(f,ul2zum(VARIABLE));
    }
    for(p=v->identifier;*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
    emit(f,"\t%s\t0\n",dwarfd1);
    emit(f,"\t%s\t%s%d\n",da,lp,l);
    if((v->storage_class==AUTO||v->storage_class==REGISTER)&&(!zmleq(l2zm(0L),v->offset)||v->reg)){
      /* no external flag for formal-parameter */
    }else{
      emit(f,"\t%s\t%d\n",dwarfd1,(v->storage_class==EXTERN));
    }
    if((!v->dfilename||!v->dline)&&(!v->filename||!v->line)) ierror(0);
    if(v->dfilename){
      dwarf2_print_uleb128(f,ul2zum((unsigned long)dwarf2_file(v->dfilename)));
      dwarf2_print_uleb128(f,ul2zum((unsigned long)v->dline));
    }else{
      dwarf2_print_uleb128(f,ul2zum((unsigned long)dwarf2_file(v->filename)));
      dwarf2_print_uleb128(f,ul2zum((unsigned long)v->line));
    }
    {  /*FIXME!*/
      obj o;
      o.flags=VAR;
      o.val.vmax=l2zm(0L);
      o.v=v;
#if HAVE_LOCATION_LISTS
      emit(f,"\t%s\t\".debug_loc\"\n",dsec);
      ++label;
      emit(f,"%s%d:\n",lp,label);
      emit(f,"\t%s\t0\n",da);
      emit(f,"\t%s\t-1\n",da);
      dwarf2_print_location(f,&o);
      emit(f,"\t%s\t0\n",da);
      emit(f,"\t%s\t0\n",da);      
      emit(f,"\t%s\t\".debug_info\"\n",dsec);
      emit(f,"\t%s\t%s%d\n",da,lp,label);
#else
      dwarf2_print_location(f,&o);
#endif      
    }
  }
}
static void dwarf2_print_comp_unit_header(FILE *f)
{
  char *p;
  extern char *copyright; /* not nice */
  extern char *inname; /* not nice */
  
  abbrev_label=++label;
  emit(f,"\t%s\t\".debug_abbrev\"\n",dsec);
  emit(f,"%s%d:\n",lp,abbrev_label);

  dwarf2_print_uleb128(f,ul2zum(COMP_UNIT));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_compile_unit));
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes)); 
  dwarf2_print_uleb128(f,ul2zum(DW_AT_language));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_stmt_list));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_producer));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_identifier_case));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(0UL));
  dwarf2_print_uleb128(f,ul2zum(0UL));

  dwarf2_print_uleb128(f,ul2zum(SUBPROGRAM));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_subprogram));
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_low_pc));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_high_pc));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_external));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_flag));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_file));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_line));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_frame_base));
#if HAVE_LOCATION_LISTS
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
#else
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_block2));
#endif
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(SUBPROGRAMVOID));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_subprogram));
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_low_pc));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_high_pc));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_external));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_flag));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_file));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_line));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_frame_base));
#if HAVE_LOCATION_LISTS
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
#else
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_block2));
#endif
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(FORMALPARAMETER));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_formal_parameter));
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_file));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_line));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_location));
#if HAVE_LOCATION_LISTS
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
#else
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_block2));
#endif
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(VARIABLE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_variable));
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_external));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_flag));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_file));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_decl_line));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_location));
#if HAVE_LOCATION_LISTS
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
#else
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_block2));
#endif
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(BASETYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_base_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_encoding));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_byte_size));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(POINTERTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_pointer_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));  

  dwarf2_print_uleb128(f,ul2zum(CONSTTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_const_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(VOLATILETYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_volatile_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(ARRAYTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_array_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(ARRAYDIM));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_subrange_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_upper_bound));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(STRUCTTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_structure_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_byte_size));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(UNIONTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_union_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_byte_size));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(STRUCTTAG));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_structure_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_byte_size));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(UNIONTAG));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_union_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_byte_size));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_udata));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(MEMBERTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_member));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_name));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_string));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_data_member_location));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_block));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(FUNCTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_subroutine_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(VOIDFUNCTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_subroutine_type));  
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_yes));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));

  dwarf2_print_uleb128(f,ul2zum(PARMTYPE));
  dwarf2_print_uleb128(f,ul2zum(DW_TAG_formal_parameter));
  dwarf2_print_uleb128(f,ul2zum(DW_CHILDREN_no));
  dwarf2_print_uleb128(f,ul2zum(DW_AT_type));
  dwarf2_print_uleb128(f,ul2zum(DW_FORM_ref_addr));
  dwarf2_print_uleb128(f,ul2zum(0L));
  dwarf2_print_uleb128(f,ul2zum(0L));


  info_start=++label;info_end=++label;
  emit(f,"\t%s\t\".debug_info\"\n",dsec);
  
  /* header */
  emit(f,"%s%d:\n",lp,info_start);
  emit(f,"\t%s\t%s%d-%s%d-4\n",dwarfd4,lp,info_end,lp,info_start);
  emit(f,"\t%s\t2\n",dwarfd2); /* version */
  emit(f,"\t%s\t%s%d\n",dwarfd4,lp,abbrev_label);
  emit(f,"\t%s\t%d\n",dwarfd1,sizeof_addr);
  
  /* compile_unit */
  dwarf2_print_uleb128(f,ul2zum(COMP_UNIT));
  dwarf2_print_uleb128(f,ul2zum(DW_LANG_C89));
  line_start=++label;line_end=++label;
  emit(f,"\t%s\t%s%d\n",da,lp,line_start);
  for(p=copyright;*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
  emit(f,"\t%s\t0\n",dwarfd1);
  emit(f,"\t%s\t%d\n",dwarfd1,DW_ID_case_sensitive);
  for(p=inname?inname:"<multiple>";*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
  emit(f,"\t%s\t0\n",dwarfd1);
}
static void dwarf2_cleanup(FILE *f)
{
  int i,line,file,label,length;char *p;
  tdwarf2_line_info *li,*m;
  Var *vl;
  struct_declaration *sd;
  emit(f,"\t%s\t\".debug_line\"\n",dsec);
  /* line info */
  emit(f,"%s%d:\n",lp,line_start);
  emit(f,"\t%s\t%s%d-%s%d-4\n",dwarfd4,lp,line_end,lp,line_start); /* length */
  emit(f,"\t%s\t2\n",dwarfd2); /* version */
  length=16;
  for(i=0;i<namecount;i++) length+=strlen(names[i])+4;
  emit(f,"\t%s\t%d\n",dwarfd4,length); /* prologue length */
  emit(f,"\t%s\t1\n",dwarfd1); /* instruction_length */
  emit(f,"\t%s\t1\n",dwarfd1); /* is_stmt */
  emit(f,"\t%s\t-10\n",dwarfd1); /* line_base */
  emit(f,"\t%s\t245\n",dwarfd1); /* line_range */
  emit(f,"\t%s\t10\n",dwarfd1); /* opcode_base */
  emit(f,"\t%s\t0\n",dwarfd1); /* standard opcode arguments */
  emit(f,"\t%s\t1\n",dwarfd1);
  emit(f,"\t%s\t1\n",dwarfd1);
  emit(f,"\t%s\t1\n",dwarfd1);
  emit(f,"\t%s\t1\n",dwarfd1);
  emit(f,"\t%s\t0\n",dwarfd1);
  emit(f,"\t%s\t0\n",dwarfd1);
  emit(f,"\t%s\t0\n",dwarfd1);
  emit(f,"\t%s\t1\n",dwarfd1);
  emit(f,"\t%s\t0\n",dwarfd1); /* include directories */
  for(i=0;i<namecount;i++){
    for(p=names[i];*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
    emit(f,"\t%s\t0\n",dwarfd1);
    emit(f,"\t%s\t0\n",dwarfd1);
    emit(f,"\t%s\t0\n",dwarfd1);
    emit(f,"\t%s\t0\n",dwarfd1);
  }
  emit(f,"\t%s\t0\n",dwarfd1); /* file names */  
  
  file=-1;line=1;label=-1;
  li=dwarf2_first_li;
  while(li){
    m=li->next;
    if(li->file!=file){
      file=li->file;
      emit(f,"\t%s\t%d\n",dwarfd1,DW_LNS_set_file);
      dwarf2_print_uleb128(f,ul2zum((long)file));
    }
    if(!li->id&&li->label!=label){
      label=li->label;
      emit(f,"\t%s\t0\n",dwarfd1); /* extended opcode */
      emit(f,"\t%s\t%d\n",dwarfd1,1+sizeof_addr); /* length */
      emit(f,"\t%s\t%d\n",dwarfd1,DW_LNE_set_address);
      emit(f,"\t%s\t%s%d\n",da,lp,label);      
    }
    if(li->id){
      emit(f,"\t%s\t0\n",dwarfd1); /* extended opcode */
      emit(f,"\t%s\t%d\n",dwarfd1,1+sizeof_addr); /* length */
      emit(f,"\t%s\t%d\n",dwarfd1,DW_LNE_set_address);
      emit(f,"\t%s\t%s%s\n",da,ip,li->id);
      free(li->id);
    }      
    if(li->line!=line){
      emit(f,"\t%s\t%d\n",dwarfd1,DW_LNS_advance_line);
      dwarf2_print_sleb128(f,l2zm((long)(li->line-line)));
      line=li->line;
    }
    emit(f,"\t%s\t%d\n",dwarfd1,DW_LNS_copy);
    free(li);
    li=m;
  }
  emit(f,"\t%s\t0,1,1\n",dwarfd1); /* end_sequence */
  emit(f,"%s%d:\n",lp,line_end);
  emit(f,"\t%s\t\".debug_info\"\n",dsec);
  for(sd=first_sd[0];sd;sd=sd->next){
    if(sd->identifier){
      static type styp;
      styp.flags=sd->typ;
      styp.exact=sd;
      sd->label=0;
      dwarf2_type(f,&styp);
    }
  }
  for(vl=first_var[0];vl;vl=vl->next)
    dwarf2_var(f,vl);
  for(vl=first_ext;vl;vl=vl->next)
    dwarf2_var(f,vl);
  emit(f,"\t%s\t0\n",dwarfd1);
  emit(f,"%s%d:\n",lp,info_end);
  for(i=0;i<namecount;i++) free(names[i]);
  free(names);
}
/* generate line info for IC p, return 0 if nothing had to be generated */
static void dwarf2_line_info(FILE *f,IC *p)
{
  static int line,file,n;
  tdwarf2_line_info *new;
  if(!f) return;
  if(p->line==0) return;
  if(!p->file) ierror(0);
  n=dwarf2_file(p->file);
  if(n!=file||p->line!=line){
    file=n;
    line=p->line;
    emit(f,"%s%d:\n",lp,++label);
    dwarf2_add_line(file,line,label,0);
  }
}

static void dwarf2_function(FILE *f,Var *v,int endlabel)
{
  char *p;int l;Var *vl;
  struct struct_identifier *si;
  /* subprogram */
  if(!f) return;
  emit(f,"\t%s\t\".debug_info\"\n",dsec);
  if(!v->vtyp||!v->vtyp->next) ierror(0);
  if((v->vtyp->next->flags&NQ)==VOID){
    dwarf2_print_uleb128(f,ul2zum(SUBPROGRAMVOID));
  }else{
    l=dwarf2_type(f,v->vtyp->next);
    dwarf2_print_uleb128(f,ul2zum(SUBPROGRAM));
  }
  for(p=ip;*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
  for(p=v->identifier;*p;p++) emit(f,"\t%s\t%d\n",dwarfd1,*p);
  emit(f,"\t%s\t0\n",dwarfd1);
  if(v->storage_class==EXTERN){
    emit(f,"\t%s\t%s%s\n",da,ip,v->identifier);
  }else{
    emit(f,"\t%s\t%s%ld\n",da,lp,zm2l(v->offset));    
  }
  emit(f,"\t%s\t%s%d\n",da,lp,endlabel);
  if((v->vtyp->next->flags&NQ)!=VOID)
    emit(f,"\t%s\t%s%d\n",da,lp,l);
  if(!v->dfilename||v->dline<=0) ierror(0);
  emit(f,"\t%s\t%d\n",dwarfd1,(v->storage_class==EXTERN));
  if(v->storage_class==EXTERN)
    dwarf2_add_line(dwarf2_file(v->dfilename),v->dline,0,v->identifier);
  else
    dwarf2_add_line(dwarf2_file(v->dfilename),v->dline,(int)zm2l(v->offset),0);
  dwarf2_print_uleb128(f,ul2zum((long)dwarf2_file(v->dfilename)));
  dwarf2_print_uleb128(f,ul2zum((long)v->dline));
#if HAVE_LOCATION_LISTS
  emit(f,"\t%s\t\".debug_loc\"\n",dsec);
  emit(f,"%s%d:\n",lp,++label);
  emit(f,"\t%s\t0\n",da);
  emit(f,"\t%s\t-1\n",da); 
  dwarf2_print_frame_location(f,v);
  emit(f,"\t%s\t0\n",da);
  emit(f,"\t%s\t0\n",da);   
  emit(f,"\t%s\t\".debug_info\"\n",dsec);
  emit(f,"\t%s\t%s%d\n",da,lp,label);
#else
  dwarf2_print_frame_location(f,v);
#endif

  /* children */

  /*FIXME: not nice */
  if(cross_module){
    if(!v->fi) ierror(0);
    for(vl=v->fi->vars;vl;vl=vl->next)
      dwarf2_var(f,vl);
  }else{
    for(vl=first_var[1];vl;vl=vl->next)
      dwarf2_var(f,vl);
    for(vl=merk_varf;vl;vl=vl->next)
      dwarf2_var(f,vl);
  }
  
  
  emit(f,"\t%s\t0\n",dwarfd1);
}
