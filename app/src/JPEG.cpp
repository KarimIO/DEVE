#include "JPEG.h"

JPEG::JPEG() {
    notString = std::vector<uint8>();
    comment = "This image was processed in DEVE.";
}

JPEG JPEG::fromByteVector(const std::vector<uint8>* byteVector) {
    JPEG returnValue = JPEG();
    std::vector<uint8> commentText = std::vector<uint8>();

    auto inComment = false;
    auto& vec = *byteVector;

    for (int i = 0; i < vec.size() - 3; i += 1) {
        if (vec[i] == 0xFF && vec[i + 1] == 0xFE) {
            inComment = true;
            i += 1;
        } 
        else if (inComment) {
            commentText.push_back(vec[i]);
        } else {
            returnValue.notString.push_back(vec[i]);
        }
    }

    returnValue.comment = RRAD::devectorizeToString(commentText);
    if (returnValue.comment.length() == 0) {
        returnValue.comment = "This image was processed in DEVE.";
    }
    return returnValue;
}

std::vector<uint8> JPEG::toByteVector() {
    auto returnValue = notString;
    auto vectorized = RRAD::vectorize(comment);
    returnValue.push_back(0xFF);
    returnValue.push_back(0xFE);
    returnValue.insert(returnValue.end(), vectorized.begin(), vectorized.end());
    returnValue.push_back(0xFF);
    returnValue.push_back(0xD9);
    return returnValue;
}
