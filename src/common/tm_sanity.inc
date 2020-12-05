/* sanity checks, only enabled if TM_ASSERT and TM_SANITY_ENABLED are defined */
#if !defined(TM_SANITY) && defined(TM_ASSERT) && defined(TM_SANITY_ENABLED)
    #define TM_SANITY TM_ASSERT
#elif !defined(TM_SANITY)
    #define TM_SANITY(...) ((void)0)
#endif