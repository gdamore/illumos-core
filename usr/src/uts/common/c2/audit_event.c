
static au_event_t	aui_fchmodat(au_event_t);
static void	aus_fchmodat(struct t_audit_data *);
static void	aus_mkdirat(struct t_audit_data *);
static void	aus_mknodat(struct t_audit_data *);
static void	auf_mknodat(struct t_audit_data *, int, rval_t *);
aui_null,	AUE_LINK,	aus_null,	/* 7 linkat */
aui_null,	AUE_SYMLINK,	aus_null,	/* 11 symlinkat */
		auf_mknod,	S2E_MLD,
aui_null,	AUE_READLINK,	aus_null,	/* 22 readlinkat */
		auf_null,	S2E_PUB,
aui_null,	AUE_MKNOD,	aus_mknodat,	/* 48 mknodat */
		auf_mknodat,	S2E_MLD,
aui_fchmodat,	AUE_NULL,	aus_fchmodat,	/* 101 fchmodat */
aui_null,	AUE_MKDIR,	aus_mkdirat,	/* 102 mkdirat */
	/*
	 * convert file pointer to file descriptor
	 *   Note: fd ref count incremented here.
	 */
	/* get path from file struct here */
	fad = F2A(fp);
	if (fad->fad_aupath != NULL) {
		au_uwrite(au_to_path(fad->fad_aupath));
	} else {
		au_uwrite(au_to_arg32(1, "no path: fd", fd));
	}

	vp = fp->f_vnode;
	audit_attributes(vp);

	/* decrement file descriptor reference count */
	releasef(fd);
}

static au_event_t
aui_fchmodat(au_event_t e)
{
	klwp_t *clwp = ttolwp(curthread);

	struct a {
		long	fd;
		long	fname;		/* char	* */
		long	fmode;
		long	flag;
	} *uap = (struct a *)clwp->lwp_ap;

	if (uap->fname == NULL)
		e = AUE_FCHMOD;
	else
		e = AUE_CHMOD;

	return (e);
}

/*ARGSUSED*/
static void
aus_fchmodat(struct t_audit_data *tad)
{
	klwp_t *clwp = ttolwp(curthread);
	uint32_t fmode;
	uint32_t fd;
	struct file  *fp;
	struct vnode *vp;
	struct f_audit_data *fad;

	struct a {
		long	fd;
		long	fname;		/* char	* */
		long	fmode;
		long	flag;
	} *uap = (struct a *)clwp->lwp_ap;

	fd = (uint32_t)uap->fd;
	fmode = (uint32_t)uap->fmode;

	au_uwrite(au_to_arg32(2, "new file mode", fmode&07777));

	if (fd == AT_FDCWD || uap->fname != NULL)	/* same as chmod() */
		return;

	/*
	 * convert file pointer to file descriptor
	 *   Note: fd ref count incremented here.
	 */
	if ((fp = getf(fd)) == NULL)
		return;

	/* get path from file struct here */
	switch (fm & (O_ACCMODE | O_CREAT | O_TRUNC)) {
	case O_SEARCH:
		e = AUE_OPEN_S;
		break;
	case O_EXEC:
		e = AUE_OPEN_E;
		break;
/*ARGSUSED*/
static void
aus_mkdirat(struct t_audit_data *tad)
{
	klwp_t *clwp = ttolwp(curthread);
	uint32_t dmode;

	struct a {
		long	fd;
		long	dirnamep;		/* char * */
		long	dmode;
	} *uap = (struct a *)clwp->lwp_ap;

	dmode = (uint32_t)uap->dmode;

	au_uwrite(au_to_arg32(2, "mode", dmode));
}

	if (error != EPERM && error != EINVAL)
	/* do the lookup to force generation of path token */
	pnamep = (caddr_t)uap->pnamep;
	tad->tad_ctrl |= TAD_NOATTRB;
	error = lookupname(pnamep, UIO_USERSPACE, NO_FOLLOW, &dvp, NULLVPP);
	if (error == 0)
		VN_RELE(dvp);
}

/*ARGSUSED*/
static void
aus_mknodat(struct t_audit_data *tad)
{
	klwp_t *clwp = ttolwp(curthread);
	uint32_t fmode;
	dev_t dev;

	struct a {
		long	fd;
		long	pnamep;		/* char * */
		long	fmode;
		long	dev;
	} *uap = (struct a *)clwp->lwp_ap;

	fmode = (uint32_t)uap->fmode;
	dev   = (dev_t)uap->dev;

	au_uwrite(au_to_arg32(2, "mode", fmode));
#ifdef _LP64
	au_uwrite(au_to_arg64(3, "dev", dev));
#else
	au_uwrite(au_to_arg32(3, "dev", dev));
#endif
}

/*ARGSUSED*/
static void
auf_mknodat(struct t_audit_data *tad, int error, rval_t *rval)
{
	klwp_t *clwp = ttolwp(curthread);
	vnode_t	*startvp;
	vnode_t	*dvp;
	caddr_t pnamep;
	int fd;

	struct a {
		long	fd;
		long	pnamep;		/* char * */
		long	fmode;
		long	dev;
	} *uap = (struct a *)clwp->lwp_ap;

	/* no error, then already path token in audit record */
	if (error != EPERM && error != EINVAL)
	fd = (int)uap->fd;
	if (pnamep == NULL ||
	    fgetstartvp(fd, pnamep, &startvp) != 0)
		return;
	error = lookupnameat(pnamep, UIO_USERSPACE, NO_FOLLOW, &dvp, NULLVPP,
	    startvp);
	if (startvp != NULL)
		VN_RELE(startvp);
		case A_GETAMASK:
			e = AUE_AUDITON_GETAMASK;
			break;
		case A_SETAMASK:
			e = AUE_AUDITON_SETAMASK;
			break;
	case AUE_AUDITON_SETAMASK:
		if (copyin((caddr_t)a2, &mask, sizeof (au_mask_t)))
				return;
		au_uwrite(au_to_arg32(
		    2, "setamask:as_success", (uint32_t)mask.as_success));
		au_uwrite(au_to_arg32(
		    2, "setamask:as_failure", (uint32_t)mask.as_failure));
		break;
	case AUE_AUDITON_GETAMASK:
	switch (sy_flags) {
	case SE_32RVAL1:
		/* FALLTHRU */
	case SE_32RVAL2|SE_32RVAL1:
		break;
	case SE_64RVAL:
		break;
	default:
		/*
		 * should never happen, seems to be an internal error
		 * in sysent => no fd, nothing to audit here, returning
		 */
		return;
	}