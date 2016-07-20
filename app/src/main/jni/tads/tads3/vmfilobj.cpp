    &CVmObjFile::getp_undef,
    &CVmObjFile::getp_open_text,
    &CVmObjFile::getp_open_data,
    &CVmObjFile::getp_open_raw,
    &CVmObjFile::getp_get_charset,
    &CVmObjFile::getp_set_charset,
    &CVmObjFile::getp_close_file,
    &CVmObjFile::getp_read_file,
    &CVmObjFile::getp_write_file,
    &CVmObjFile::getp_read_bytes,
    &CVmObjFile::getp_write_bytes,
    &CVmObjFile::getp_get_pos,
    &CVmObjFile::getp_set_pos,
    &CVmObjFile::getp_set_pos_end,
    &CVmObjFile::getp_open_res_text,
    &CVmObjFile::getp_open_res_raw,
    &CVmObjFile::getp_get_size
    VMOBJFILE_OPEN_RES_RAW = 15
/* ------------------------------------------------------------------------ */
/*
 *   Special filename designators 
 */

/* library defaults file */
#define SFID_LIB_DEFAULTS    0x0001


    /* instantiate the object */
                               vm_obj_id_t charset, osfildef *fp,
                               unsigned long flags, int mode, int access,
                               int create_readbuf,
                               unsigned long res_start, unsigned long res_end)

    /* instantiate the object */
    new (vmg_ id) CVmObjFile(vmg_ charset, fp, flags, mode, access,
                             create_readbuf, res_start, res_end);
CVmObjFile::CVmObjFile(VMG_ vm_obj_id_t charset, osfildef *fp,
                       unsigned long flags, int mode, int access,
                       int create_readbuf,
                       unsigned long res_start, unsigned long res_end)
    alloc_ext(vmg_ charset, fp, flags, mode, access, create_readbuf,
              res_start, res_end);
void CVmObjFile::alloc_ext(VMG_ vm_obj_id_t charset, osfildef *fp,
                           int create_readbuf,
                           unsigned long res_start, unsigned long res_end)
    size_t siz;

    siz = sizeof(vmobjfile_ext_t);
    get_ext()->res_start = res_start;
    get_ext()->res_end = res_end;
        get_ext()->readbuf = (vmobjfile_readbuf_t *)(get_ext() + 1);
        get_ext()->readbuf->rem = 0;
        get_ext()->readbuf->ptr.set(0);
            osfcls(get_ext()->fp);
    switch(G_meta_table
           ->prop_to_vector_idx(metaclass_reg_->get_reg_idx(), prop))
        return s_getp_open_text(vmg_ result, argc, FALSE);
        return s_getp_open_data(vmg_ result, argc);
        return s_getp_open_raw(vmg_ result, argc, FALSE);
        return s_getp_open_text(vmg_ result, argc, TRUE);
        return s_getp_open_raw(vmg_ result, argc, TRUE);
    vm_obj_id_t charset;
    unsigned long flags;
    int mode;
    int access;

    charset = vmb_get_objid(ptr);
    mode = (unsigned char)*ptr++;
    access = (unsigned char)*ptr++;
    flags = t3rp4u(ptr);
     *   Initialize our extension - we have no underlying native file
     *   handle, since the file is out of sync by virtue of being loaded
     *   from a previously saved image state.  Note that we don't need a
     *   read buffer because the file is inherently out of sync and thus
     *   cannot be read.  
    alloc_ext(vmg_ charset, 0, flags, mode, access, FALSE, 0, 0);
    /* files are always transient, so should never be savd */
void CVmObjFile::note_file_seek(VMG_ vm_obj_id_t self, int is_explicit)
void CVmObjFile::switch_read_write_mode(int writing)
            osfseek(get_ext()->fp, osfpos(get_ext()->fp), OSFSK_SET);
     *   we need to seek if we perform the opposite type of application
     *   after this one 
 *   Retrieve the filename and access mode arguments. 
void CVmObjFile::get_filename_and_access(VMG_ char *fname, size_t fname_siz,
                                         int *access, int is_resource_file)
    int is_special_file = FALSE;
    
    /* 
     *   check to see if we have an explicit filename string, or an integer
     *   giving a special system file ID 
    if (G_stk->get(0)->typ == VM_INT)
        char path[OSFNMAX];
        /* start with no path, in case we have trouble retrieving it */
        path[0] = '\0';
        
        /* we have an integer, which is a special file designator */
        switch (CVmBif::pop_int_val(vmg0_))
        {
        case SFID_LIB_DEFAULTS:
            /* get the system application data path */
            G_host_ifc->get_special_file_path(path, sizeof(path),
                                              OS_GSP_T3_APP_DATA);

            /* add the filename */
            os_build_full_path(fname, fname_siz, path, "settings.txt");
            break;

        default:
            /* invalid filename value */
        }

        /* note that we have a special file, for file safety purposes */
        is_special_file = TRUE;
    }
    else
    {
        /* we must have an explicit filename string - pop it */
        CVmBif::pop_str_val_fname(vmg_ fname, fname_siz);
    /* 
     *   retrieve the access mode; if it's a resource file, the mode is
     *   implicitly 'read' 
     */
    if (is_resource_file)
        *access = VMOBJFILE_ACCESS_READ;
    else
        *access = CVmBif::pop_int_val(vmg0_);
    /*
     *   If this isn't a special file, then check the file safety mode to
     *   ensure this operation is allowed.  Reading resources is always
     *   allowed, regardless of the safety mode, since resources are
     *   read-only and are inherently constrained in the paths they can
     *   access.  Likewise, special files bypass the safety settings, because
     *   the interpreter controls the names and locations of these files,
     *   ensuring that they're inherently safe.  
     */
    if (!is_resource_file && !is_special_file)
        check_safety_for_open(vmg_ fname, *access);
/* ------------------------------------------------------------------------ */
 *   Static property evaluator - open a text file 
int CVmObjFile::s_getp_open_text(VMG_ vm_val_t *retval, uint *in_argc,
                                 int is_resource_file)
    uint argc = (in_argc != 0 ? *in_argc : 0);
    static CVmNativeCodeDesc desc_file(2, 1);
    static CVmNativeCodeDesc desc_res(1, 1);
    char fname[OSFNMAX];
    int access;
    vm_obj_id_t cset_obj;
    osfildef *fp;
    int create_readbuf;
    unsigned long res_start;
    unsigned long res_end;
    unsigned int flags;
    /* check arguments */
    if (get_prop_check_argc(retval, in_argc,
                            is_resource_file ? &desc_res : &desc_file))
        return TRUE;
    /* initialize the flags to indicate a text-mode file */
    flags = 0;
    /* add the resource-file flag if appropriate */
    if (is_resource_file)
        flags |= VMOBJFILE_IS_RESOURCE;
    /* presume we can use the entire file */
    res_start = 0;
    res_end = 0;
    /* retrieve the filename */
    get_filename_and_access(vmg_ fname, sizeof(fname),
                            &access, is_resource_file);
    /* presume we won't need a read buffer */
    create_readbuf = FALSE;
    /* if there's a character set name or object, retrieve it */
    if (argc > 2)
        /* 
         *   check to see if it's a CharacterSet object; if it's not, it
         *   must be a string giving the character set name 
         */
        if (G_stk->get(0)->typ == VM_OBJ
            && CVmObjCharSet::is_charset(vmg_ G_stk->get(0)->val.obj))
            /* retrieve the CharacterSet reference */
            cset_obj = CVmBif::pop_obj_val(vmg0_);
        else
            const char *str;
            size_t len;
            
            /* it's not a CharacterSet, so it must be a character set name */
            str = G_stk->get(0)->get_as_string(vmg0_);
            if (str == 0)
                err_throw(VMERR_BAD_TYPE_BIF);
            /* get the length and skip the length prefix */
            len = vmb_get_len(str);
            str += VMB_LEN;
            /* create a mapper for the given name */
            cset_obj = CVmObjCharSet::create(vmg_ FALSE, str, len);
        }
    }
    else
        /* no character set is specified - use US-ASCII by default */
        cset_obj = CVmObjCharSet::create(vmg_ FALSE, "us-ascii", 8);
    /* push the character map object onto the stack for gc protection */
    G_stk->push()->set_obj(cset_obj);
    /* open the file for reading or writing, as appropriate */
    switch(access)
    case VMOBJFILE_ACCESS_READ:
        /* open a resource file or file system file, as appropriate */
        if (is_resource_file)
        {
            unsigned long res_len;
            
            /* it's a resource - open it */
            fp = G_host_ifc->find_resource(fname, strlen(fname), &res_len);
            /* 
             *   if we found the resource, note the start and end seek
             *   positions, so we can limit reading of the underlying file
             *   to the section that contains the resource data 
             */
            if (fp != 0)
            {
                /* the file is initially at the start of the resource data */
                res_start = osfpos(fp);
                /* note the offset of the first byte after the resource */
                res_end = res_start + res_len;
            }
        }
        else
        {
            /* 
             *   Not a resource - open an ordinary text file for reading.
             *   Even though we're going to treat the file as a text file,
             *   open it in binary mode, since we'll do our own universal
             *   newline translations; this allows us to work with files in
             *   any character set, and using almost any newline
             *   conventions, so files copied from other systems will be
             *   fully usable even if they haven't been fixed up to local
             *   conventions.  
             */
            fp = osfoprb(fname, OSFTTEXT);
        }
        /* make sure we opened it successfully */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_not_found_exc,
                                           0, "file not found");
        /* we need a read buffer */
        create_readbuf = TRUE;
        break;
    case VMOBJFILE_ACCESS_WRITE:
        /* open for writing */
        fp = osfopwb(fname, OSFTTEXT);
        /* make sure we created it successfully */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_creation_exc,
                                           0, "error creating file");
        break;
    case VMOBJFILE_ACCESS_RW_KEEP:
        /* open for read/write, keeping existing contents */
        fp = osfoprwb(fname, OSFTTEXT);
        /* make sure we were able to find or create the file */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_open_exc,
                                           0, "error opening file");
        break;
    case VMOBJFILE_ACCESS_RW_TRUNC:
        /* open for read/write, truncating existing contents */
        fp = osfoprwtb(fname, OSFTTEXT);
        /* make sure we were successful */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_open_exc,
                                           0, "error opening file");
        break;
    default:
        fp = 0;
        err_throw(VMERR_BAD_VAL_BIF);

    /* create our file object */
    retval->set_obj(create(vmg_ FALSE, cset_obj, fp, flags,
                           VMOBJFILE_MODE_TEXT, access, create_readbuf,
                           res_start, res_end));

    /* discard gc protection */
    G_stk->discard();
int CVmObjFile::s_getp_open_data(VMG_ vm_val_t *retval, uint *argc)
    return open_binary(vmg_ retval, argc, VMOBJFILE_MODE_DATA, FALSE);
                                int is_resource_file)
                       is_resource_file);
int CVmObjFile::open_binary(VMG_ vm_val_t *retval, uint *argc, int mode,
                            int is_resource_file)
    char fname[OSFNMAX];
    int access;
    osfildef *fp;
    unsigned long res_start;
    unsigned long res_end;
    unsigned int flags;

    /* check arguments */
    if (get_prop_check_argc(retval, argc,
    /* initialize the flags */
    flags = 0;

    /* set the resource-file flag, if appropriate */
    if (is_resource_file)
        flags |= VMOBJFILE_IS_RESOURCE;

    /* presume we can use the entire file */
    res_start = 0;
    res_end = 0;

    get_filename_and_access(vmg_ fname, sizeof(fname),
                            &access, is_resource_file);
    /* open the file in binary mode, with the desired access type */
    switch(access)
    case VMOBJFILE_ACCESS_READ:
        /* open the resource or ordinary file, as appropriate */
        if (is_resource_file)
            unsigned long res_len;

            /* it's a resource - open it */
            fp = G_host_ifc->find_resource(fname, strlen(fname), &res_len);

            /* 
             *   if we found the resource, note the start and end seek
             *   positions, so we can limit reading of the underlying file
             *   to the section that contains the resource data 
             */
            if (fp != 0)
                /* the file is initially at the start of the resource data */
                res_start = osfpos(fp);

                /* note the offset of the first byte after the resource */
                res_end = res_start + res_len;
        else
        {
            /* open the ordinary file in binary mode for reading only */
            fp = osfoprb(fname, OSFTBIN);
        }
        /* make sure we were able to find it and open it */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_not_found_exc,
                                           0, "file not found");
        break;
    case VMOBJFILE_ACCESS_WRITE:
        /* open in binary mode for writing only */
        fp = osfopwb(fname, OSFTBIN);
        /* make sure we were able to create the file successfully */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_creation_exc,
                                           0, "error creating file");
        break;
    case VMOBJFILE_ACCESS_RW_KEEP:
        /* open for read/write, keeping existing contents */
        fp = osfoprwb(fname, OSFTBIN);
        /* make sure we were able to find or create the file */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_open_exc,
                                           0, "error opening file");
        break;
    case VMOBJFILE_ACCESS_RW_TRUNC:
        /* open for read/write, truncating existing contents */
        fp = osfoprwtb(fname, OSFTBIN);
        /* make sure we were successful */
        if (fp == 0)
            G_interpreter->throw_new_class(vmg_ G_predef->file_open_exc,
                                           0, "error opening file");
        break;
    default:
        fp = 0;
        err_throw(VMERR_BAD_VAL_BIF);
    /* create our file object */
    retval->set_obj(create(vmg_ FALSE, VM_INVALID_OBJ, fp,
                           flags, mode, access, FALSE, res_start, res_end));
    /* handled */
    return TRUE;
/* ------------------------------------------------------------------------ */
 *   Check the safety settings to determine if an open operation is allowed.
 *   If the access is not allowed, we'll throw an error.  
void CVmObjFile::check_safety_for_open(VMG_ const char *fname, int access)
    int safety;
    int in_same_dir;
    
    safety = G_host_ifc->get_io_safety();
     *   Check to see if the file is in the current directory - if not, we
     *   may have to disallow the operation based on safety level settings.
     *   If the file has any sort of directory prefix, assume it's not in
     *   the same directory; if not, it must be.  This is actually overly
     *   conservative, since the path may be a relative path or even an
     *   absolute path that points to the current directory, but the
     *   important thing is whether we're allowing files to specify paths at
     *   all.  
    in_same_dir = (os_get_root_name((char *)fname) == fname);

    /* check for conformance with the safety level setting */
        /*
         *   we want only read access - we can't read at all if the safety
         *   level isn't READ_CUR or below, and we must be at level
         *   READ_ANY_WRITE_CUR or lower to read from a file not in the
         *   current directory 
        if (safety > VM_IO_SAFETY_READ_CUR
            || (!in_same_dir && safety > VM_IO_SAFETY_READ_ANY_WRITE_CUR))
        {
            /* this operation is not allowed - throw an error */
            G_interpreter->throw_new_class(vmg_ G_predef->file_safety_exc,
                                           0, "prohibited file access");
        }
        
         *   writing - we must be safety level of at least READWRITE_CUR to
         *   write at all, and we must be at level MINIMUM to write a file
         *   that's not in the current directory 
        if (safety > VM_IO_SAFETY_READWRITE_CUR
            || (!in_same_dir && safety > VM_IO_SAFETY_MINIMUM))
        {
            /* this operation is not allowed - throw an error */
            G_interpreter->throw_new_class(vmg_ G_predef->file_safety_exc,
                                           0, "prohibited file access");
        }

    /* check arguments */
    /* make sure it's really a character set object */
    if (G_stk->get(0)->typ != VM_NIL
        && (G_stk->get(0)->typ != VM_OBJ
            || !CVmObjCharSet::is_charset(vmg_ G_stk->get(0)->val.obj)))
        err_throw(VMERR_BAD_TYPE_BIF);

    /* remember the new character set */
    if (G_stk->get(0)->typ == VM_NIL)
        get_ext()->charset = VM_INVALID_OBJ;
    else
        get_ext()->charset = G_stk->get(0)->val.obj;
    /* discard the argument */
    /* close the underlying system file */
    osfcls(get_ext()->fp);
    /* forget the underlying system file, since it's no longer valid */
    note_file_seek(vmg_ self, FALSE);
    switch_read_write_mode(FALSE);
    CVmObjString *str;
    size_t str_len;
    osfildef *fp = get_ext()->fp;
    CCharmapToUni *charmap;
    int is_res_file = ((get_ext()->flags & VMOBJFILE_IS_RESOURCE) != 0);

    /* we haven't yet constructed a string */
    str = 0;
    str_len = 0;
    charmap = ((CVmObjCharSet *)vm_objp(vmg_ get_ext()->charset))
              ->get_to_uni(vmg0_);
    /* assume we'll fail to read anything, in which case we'll return nil */
    retval->set_nil();
     *   push the nil value - we'll always keep our intermediate value on
     *   the stack so that the garbage collector will know it's referenced 
        wchar_t found_nl;
        char *start;
        size_t new_len;
        size_t nl_len;

        /* replenish the read buffer if it's empty */
        if (readbuf->rem == 0
            && !readbuf->refill(charmap, fp, is_res_file, get_ext()->res_end))
        /* note where we started this chunk */
        start = readbuf->ptr.getptr();

        /* scan for and remove any trailing newline */
        for (found_nl = '\0' ; readbuf->rem != 0 ;
             readbuf->ptr.inc(&readbuf->rem))
        {
            wchar_t cur;
            
            /* get the current character */
            cur = readbuf->ptr.getch();

            /* 
             *   check for a newline (note that 0x2028 is the unicode line
             *   separator character) 
             */
            if (cur == '\n' || cur == '\r' || cur == 0x2028)
            {
                /* note the newline */
                found_nl = cur;
                
                /* no need to look any further */
                break;
            }
        }
        
        /* note the length of the current segment */
        new_len = readbuf->ptr.getptr() - start;
        
        /* 
         *   if there's a newline character, include an extra byte for the
         *   '\n' we'll include in the result 
         */
        nl_len = (found_nl != '\0');
        
        /* 
         *   If this is our first segment, construct a new string from this
         *   chunk; otherwise, add to the existing string.
         *   
         *   Note that in either case, if we found a newline in the buffer,
         *   we will NOT add the actual newline we found to the result
         *   string.  Rather, we'll add a '\n' character to the result
         *   string, no matter what kind of newline we found.  This ensures
         *   that the data read uses a consistent format, regardless of the
         *   local system convention where the file was created.  
         */
        if (str == 0)
            /* create our first segment's string */
            retval->set_obj(CVmObjString::
                            create(vmg_ FALSE, new_len + nl_len));
            str = (CVmObjString *)vm_objp(vmg_ retval->val.obj);
            
            /* copy the segment into the string object */
            memcpy(str->cons_get_buf(), start, new_len);

            /* add a '\n' if we found a newline */
            if (found_nl != '\0')
                *(str->cons_get_buf() + new_len) = '\n';
            
            /* this is the length of the string so far */
            str_len = new_len + nl_len;
            
            /* 
             *   replace the stack placeholder with our string, so the
             *   garbage collector will know it's still in use 
             */
            G_stk->discard();
            G_stk->push(retval);
            CVmObjString *new_str;
            
            /* 
             *   create a new string to hold the contents of the old string
             *   plus the new buffer 
             */
            retval->set_obj(CVmObjString::create(vmg_ FALSE,
                str_len + new_len + nl_len));
            new_str = (CVmObjString *)vm_objp(vmg_ retval->val.obj);
            
            /* copy the old string into the new string */
            memcpy(new_str->cons_get_buf(),
                   str->get_as_string(vmg0_) + VMB_LEN, str_len);

            /* add the new chunk after the copy of the old string */
            memcpy(new_str->cons_get_buf() + str_len, start, new_len);

            /* add the newline if necessary */
            if (found_nl != '\0')
                *(new_str->cons_get_buf() + str_len + new_len) = '\n';
            
            /* the new string now replaces the old string */
            str = new_str;
            str_len += new_len + nl_len;
            
            /* 
             *   replace our old intermediate value on the stack with the
             *   new string - the old string isn't needed any more, so we
             *   can leave it unreferenced, but we are still using the new
             *   string 
             */
            G_stk->discard();
            G_stk->push(retval);
        /* if we found a newline in this segment, we're done */
        if (found_nl != '\0')
        {
            /* skip the newline in the input */
            readbuf->ptr.inc(&readbuf->rem);
            /* replenish the read buffer if it's empty */
            if (readbuf->rem == 0)
                readbuf->refill(charmap, fp, is_res_file, get_ext()->res_end);

            /* 
             *   check for a complementary newline character, for systems
             *   that use \n\r or \r\n pairs 
             */
            if (readbuf->rem != 0)
            {
                wchar_t nxt;
                
                /* get the next character */
                nxt = readbuf->ptr.getch();
                
                /* check for a complementary character */
                if ((found_nl == '\n' && nxt == '\r')
                    || (found_nl == '\r' && nxt == '\n'))
                {
                    /* 
                     *   we have a pair sequence - skip the second character
                     *   of the sequence 
                     */
                    readbuf->ptr.inc(&readbuf->rem);
                }
            }
            
            /* we've found the newline, so we're done with the string */
        }
    osfildef *fp = get_ext()->fp;
    if (osfrb(fp, buf, 1))
    switch((vm_datatype_t)buf[0])
        if (osfrb(fp, buf, 4))
        retval->set_int(osrp4(buf));
        if (osfrb(fp, buf, 4))
        if (osfrb(fp, buf, 2))
        if (osfrb(fp, str_obj->cons_get_buf(), osrp2(buf) - 2))
/* ------------------------------------------------------------------------ */
 *   Property evaluator - write to the file
int CVmObjFile::getp_write_file(VMG_ vm_obj_id_t self, vm_val_t *retval,
                                uint *argc)
    static CVmNativeCodeDesc desc(1);
    const vm_val_t *argval;
    /* check arguments */
    if (get_prop_check_argc(retval, argc, &desc))
        return TRUE;
    /* 
     *   get a pointer to the argument value, but leave it on the stack
     *   for now to protect against losing it in garbage collection 
     */
    argval = G_stk->get(0);
    switch_read_write_mode(TRUE);
    constp = CVmObjString::cvt_to_str(vmg_ &new_str,
                                      conv_buf, sizeof(conv_buf),
                                      val, 10);
    osfildef *fp = get_ext()->fp;
        oswp4(buf + 1, val->val.intval);
        if (osfwb(fp, buf, 5))
        if (osfwb(fp, buf, 5))
        if (osfwb(fp, buf, 1))
        if (osfwb(fp, buf, 2))
        if (osfwb(fp, constp + VMB_LEN, vmb_get_len(constp)))
            if (osfwb(fp, buf, 1))
            if (osfwb(fp, buf, 1))
        if (osfwb(fp, buf, 1))
    int is_res_file = ((get_ext()->flags & VMOBJFILE_IS_RESOURCE) != 0);
    note_file_seek(vmg_ self, FALSE);
    switch_read_write_mode(FALSE);

    /* 
     *   limit the reading to the remaining data in the file, if it's a
     *   resource file 
     */
    if (is_res_file)
    {
        unsigned long cur_seek_pos;

        /* check to see where we are relative to the end of the resource */
        cur_seek_pos = osfpos(get_ext()->fp);
        if (cur_seek_pos >= get_ext()->res_end)
        {
            /* we're already past the end - there's nothing left */
            len = 0;
        }
        else
        {
            unsigned long limit;

            /* calculate the limit */
            limit = get_ext()->res_end - cur_seek_pos;

            /* apply the limit if the request exceeds it */
            if (len > limit)
                len = limit;
        }
    }
    retval->set_int(arr->read_from_file(get_ext()->fp, idx, len));
    vm_val_t arr_val;
    CVmObjByteArray *arr;
    unsigned long idx;
    /* make sure we are allowed to perform operations on the file */
    /* check that we have write access */
    check_write_access(vmg0_);
    /* make sure the byte array argument is really a byte array */
    G_stk->pop(&arr_val);
    if (arr_val.typ != VM_OBJ
        || !CVmObjByteArray::is_byte_array(vmg_ arr_val.val.obj))
    
    /* we know it's a byte array, so we can simply cast it */
    arr = (CVmObjByteArray *)vm_objp(vmg_ arr_val.val.obj);

    /* assume we'll write the entire byte array */
    idx = 1;
    len = arr->get_element_count();
        idx = (unsigned long)CVmBif::pop_int_val(vmg0_);
        len = (unsigned long)CVmBif::pop_int_val(vmg0_);
    /* push a self-reference for gc protection */
    switch_read_write_mode(TRUE);
    /* 
     *   write the bytes to the file - on success (zero write_to_file
     *   return), return nil, on failure (non-zero write_to_file return),
     *   return true 
     */
    if (arr->write_to_file(get_ext()->fp, idx, len))
        /* we failed to write the bytes - throw an I/O exception */
        G_interpreter->throw_new_class(vmg_ G_predef->file_io_exc,
                                       0, "file I/O error");
    G_stk->discard();
    unsigned long cur_pos;
    /* get the current seek position */
    cur_pos = osfpos(get_ext()->fp);
    /* if this is a resource file, adjust for the base offset */
    cur_pos -= get_ext()->res_start;
    /* return the seek position */
    retval->set_int(cur_pos);
    /* handled */
    return TRUE;
    int is_res_file = ((get_ext()->flags & VMOBJFILE_IS_RESOURCE) != 0);
    note_file_seek(vmg_ self, TRUE);
    /* adjust for the resource base offset */
    pos += get_ext()->res_start;

    /* 
     *   if this is a resource file, move the position at most to the first
     *   byte after the end of the resource 
     */
    if (is_res_file && pos > get_ext()->res_end)
        pos = get_ext()->res_end;

    osfseek(get_ext()->fp, pos, OSFSK_SET);
    int is_res_file = ((get_ext()->flags & VMOBJFILE_IS_RESOURCE) != 0);
    note_file_seek(vmg_ self, TRUE);
    /* handle according to whether it's a resource or not */
    if (is_res_file)
    {
        /* resource - seek to the first byte after the resource data */
        osfseek(get_ext()->fp, get_ext()->res_end, OSFSK_SET);
    }
    else
    {
        /* normal file - simply seek to the end of the file */
        osfseek(get_ext()->fp, 0, OSFSK_END);
    }
    int is_res_file = ((get_ext()->flags & VMOBJFILE_IS_RESOURCE) != 0);
    note_file_seek(vmg_ self, TRUE);
    /* handle according to whether it's a resource or not */
    if (is_res_file)
        /* resource - we know the size from the resource descriptor */
        retval->set_int(get_ext()->res_end - get_ext()->res_start + 1);
        osfildef *fp = get_ext()->fp;
        unsigned long cur_pos;
        /* 
         *   It's a normal file.  Remember the current seek position, then
         *   seek to the end of the file.  
         */
        cur_pos = osfpos(fp);
        osfseek(fp, 0, OSFSK_END);
        /* the current position gives us the length of the file */
        retval->set_int(osfpos(fp));
        /* seek back to where we started */
        osfseek(fp, cur_pos, OSFSK_SET);
    }
int vmobjfile_readbuf_t::refill(CCharmapToUni *charmap,
                                osfildef *fp, int is_res_file,
                                unsigned long res_seek_end)
    unsigned long read_limit;
    /* if the buffer isn't empty, ignore the request */
        return TRUE;
    /* presume there's no read limit */
    read_limit = 0;
    /* if it's a resource file, limit the size */
    if (is_res_file)
        unsigned long cur_seek_ofs;
        /* make sure we're not already past the end */
        cur_seek_ofs = osfpos(fp);
        if (cur_seek_ofs >= res_seek_end)

        /* calculate the amount of data remaining in the resource */
        read_limit = res_seek_end - cur_seek_ofs;
    
    /* read the text */
    rem = charmap->read_file(fp, buf, sizeof(buf), read_limit);
    /* read from the start of the buffer */
    ptr.set(buf);
    /* indicate that we have more data to read */
    return (rem != 0);

