#if defined(__cplusplus) && defined(TM_USE_RESOURCE_PTR)

TMU_DEF bool tml::valid_resource(const tmu_contents& resource) { return resource.data && resource.capacity > 0; }
TMU_DEF void tml::destroy_resource(tmu_contents* resource) { tmu_destroy_contents(resource); }

TMU_DEF bool tml::valid_resource(const tmu_contents_result& resource) { return resource.ec == TM_OK; }
TMU_DEF void tml::destroy_resource(tmu_contents_result* resource) {
    if (resource) {
        tmu_destroy_contents(&resource->contents);
        *resource = tmu_contents_result();
    }
}

TMU_DEF bool tml::valid_resource(const tmu_utf8_contents_result& resource) { return resource.ec == TM_OK; }
TMU_DEF void tml::destroy_resource(tmu_utf8_contents_result* resource) {
    if (resource) {
        tmu_destroy_contents(&resource->contents);
        *resource = tmu_utf8_contents_result();
    }
}

#ifndef TMU_NO_FILE_IO
TMU_DEF bool tml::valid_resource(const tmu_utf8_command_line& resource) { return resource.internal_buffer; }
TMU_DEF void tml::destroy_resource(tmu_utf8_command_line* resource) { tmu_utf8_destroy_command_line(resource); }

TMU_DEF bool tml::valid_resource(const tmu_utf8_command_line_result& resource) { return resource.ec == TM_OK; }
TMU_DEF void tml::destroy_resource(tmu_utf8_command_line_result* resource) {
    if (resource) {
        tmu_utf8_destroy_command_line(&resource->command_line);
        *resource = tmu_utf8_command_line_result();
    }
}
#endif

#endif /* defined(__cplusplus) && defined(TM_USE_RESOURCE_PTR) */