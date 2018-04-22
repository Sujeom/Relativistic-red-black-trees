
void rp_free(void *lock, void (*func)(void *ptr), void *ptr)
{
    rp_lock_t *rp_lock = (rp_lock_t *)lock;
    int head;

    assert(ptr != NULL);
#ifdef DEBUG
    rbnode_invalid((rbnode_t *)ptr, 6);
#endif
#ifdef MULTIWRITERS
    // we need to loop until we have the write_lock AND space for our block
    // If there isn't space, we need to release the write_lock to allow
    // the polling thread to free space

    write_lock(lock);
    while (rp_lock->block.head >= RCU_MAX_BLOCKS)
    {
        write_unlock(lock);
        //assert( (rp_lock->reader_count_and_flag & 0x0001) == 0);
        // wait for polling thread to free memory
        lock_mb();

        write_lock(lock);
    }

    assert(rp_lock->block.head >= 0 && rp_lock->block.head < RCU_MAX_BLOCKS);
#else
    if (rp_lock->block.head >= BLOCKS_FOR_FREE) 
    {
        Thread_Stats[STAT_FREE_SYNC]++;
        rp_wait_grace_period(lock);
    }
#endif

    /*
    {
        int ii;
        for (ii=0; ii<rp_lock->block.head; ii++)
        {
            assert(ptr != rp_lock->block.block[ii].block);
        }
    }
    */

    Thread_Stats[STAT_FREE]++;
    head = rp_lock->block.head;
    assert(head < RCU_MAX_BLOCKS);
    rp_lock->block.block[head].block = ptr;
    rp_lock->block.block[head].func = func;
    head++;
    rp_lock->block.head = head;

#ifdef MULTIWRITERS
    write_unlock(lock);
#endif
}

rbnode_t *rbnode_copy(rbnode_t *node)
{
	rbnode_t *newnode;

    if (node == NULL) return NULL;

	newnode = (rbnode_t *)rb_alloc();
#ifdef STM
    STORE(newnode->key, LOAD(node->key));
    STORE(newnode->value, LOAD(node->value));
    STORE(newnode->left, LOAD(node->left));
    STORE(newnode->right, LOAD(node->right));
    STORE(newnode->parent, LOAD(node->parent));
    STORE(newnode->color, LOAD(node->color));

    // the following aren't used except for FG and AVL: lock, height, changeOVL

	if (LOAD(node->left)  != NULL)  STORE(LOAD(node->left)->parent, newnode);
	if (LOAD(node->right)  != NULL) STORE(LOAD(node->right)->parent, newnode);
#else
	memcpy(newnode, node, sizeof(rbnode_t));
#endif
    // NOTE: not multi-writer safe, but we'll ignore that. Index isn't that important
	newnode->index = Index++;

#ifdef STM
	if (LOAD(node->left)  != NULL) STORE(LOAD(node->left)->parent, newnode);
	if (LOAD(node->right) != NULL) STORE(LOAD(node->right)->parent, newnode);
#elif !defined(NO_GRACE_PERIOD)
	if (node->left  != NULL) node->left->parent = newnode;
	if (node->right != NULL) node->right->parent = newnode;
#endif

#ifdef FG_LOCK
    newnode->lock = lock_init();
#endif

	return newnode;
}