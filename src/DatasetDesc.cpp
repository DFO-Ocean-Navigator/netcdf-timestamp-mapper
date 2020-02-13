#include "DatasetDesc.hpp"
#include "FileReaders/NCFileReader.hpp"

#include "Utils/ProgressBar.hpp"

namespace tsm::ds {

/***********************************************************************************/
DatasetDesc::DatasetDesc(const std::vector<fs::path>& filePaths, const DATASET_TYPE type) : m_datasetType{ type } {
    m_ncFiles.reserve(filePaths.size());

    tsm::utils::ProgressBar pb{ filePaths.size() };
    for (auto i = 0; i < filePaths.size(); ++i) {
        //createAndAppendDataFileDesc(filePaths[i]);
        NCFileReader r{ filePaths[i] };
        if (const auto& desc{ r.getDataFileDesc() }; desc) {
            m_ncFiles.emplace_back(desc);
        }

        ++pb;
    }
}

} // namespace tsm::ds
