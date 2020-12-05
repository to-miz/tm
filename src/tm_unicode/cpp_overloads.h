#if defined(__cplusplus) && defined(TM_USE_RESOURCE_PTR)
namespace tml {
TMU_DEF bool valid_resource(const tmu_contents& resource);
TMU_DEF void destroy_resource(tmu_contents* resource);

TMU_DEF bool valid_resource(const tmu_contents_result& resource);
TMU_DEF void destroy_resource(tmu_contents_result* resource);

TMU_DEF bool valid_resource(const tmu_utf8_contents_result& resource);
TMU_DEF void destroy_resource(tmu_utf8_contents_result* resource);

#ifndef TMU_NO_FILE_IO
TMU_DEF bool valid_resource(const tmu_utf8_command_line& resource);
TMU_DEF void destroy_resource(tmu_utf8_command_line* resource);

TMU_DEF bool valid_resource(const tmu_utf8_command_line_result& resource);
TMU_DEF void destroy_resource(tmu_utf8_command_line_result* resource);
#endif

}
#endif /* defined(__cplusplus) && defined(TM_USE_RESOURCE_PTR) */