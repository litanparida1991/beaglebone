#ifndef _ASM_POWERPC_PGALLOC_H
#define _ASM_POWERPC_PGALLOC_H
#ifdef __KERNEL__

#include <linux/mm.h>

#ifdef CONFIG_PPC_BOOK3E
extern void tlb_flush_pgtable(struct mmu_gather *tlb, unsigned long address);
#else /* CONFIG_PPC_BOOK3E */
static inline void tlb_flush_pgtable(struct mmu_gather *tlb,
				     unsigned long address)
{
}
#endif /* !CONFIG_PPC_BOOK3E */

static inline void pte_free_kernel(struct mm_struct *mm, pte_t *pte)
{
	free_page((unsigned long)pte);
}

static inline void pte_free(struct mm_struct *mm, pgtable_t ptepage)
{
	pgtable_page_dtor(ptepage);
	__free_page(ptepage);
}

#ifdef CONFIG_PPC64
#include <asm/pgalloc-64.h>
#else
#include <asm/pgalloc-32.h>
#endif

#ifdef CONFIG_SMP
struct mmu_gather;
extern void tlb_remove_table(struct mmu_gather *, void *);

static inline void pgtable_free_tlb(struct mmu_gather *tlb, void *table, int shift)
{
	unsigned long pgf = (unsigned long)table;
	BUG_ON(shift > MAX_PGTABLE_INDEX_SIZE);
	pgf |= shift;
	tlb_remove_table(tlb, (void *)pgf);
}

static inline void __tlb_remove_table(void *_table)
{
	void *table = (void *)((unsigned long)_table & ~MAX_PGTABLE_INDEX_SIZE);
	unsigned shift = (unsigned long)_table & MAX_PGTABLE_INDEX_SIZE;

	pgtable_free(table, shift);
}
#else /* CONFIG_SMP */
static inline void pgtable_free_tlb(struct mmu_gather *tlb, void *table, unsigned shift)
{
	pgtable_free(table, shift);
}
#endif /* !CONFIG_SMP */

static inline void __pte_free_tlb(struct mmu_gather *tlb, struct page *ptepage,
				  unsigned long address)
{
	tlb_flush_pgtable(tlb, address);
	pgtable_page_dtor(ptepage);
	pgtable_free_tlb(tlb, page_address(ptepage), 0);
}

#endif /* __KERNEL__ */
#endif /* _ASM_POWERPC_PGALLOC_H */
