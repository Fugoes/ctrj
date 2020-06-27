#include <chrono>
#include <fstream>
#include <iostream>

#include "../ctrj/value.hpp"
#include "../ctrj/reader.hpp"
#include "../ctrj/writer.hpp"

#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace {

const char _areaNames[] = "areaNames";
const char _audienceSubCategoryNames[] = "audienceSubCategoryNames";
const char _blockNames[] = "blockNames";
const char _events[] = "events";
const char _description[] = "description";
const char _id[] = "id";
const char _logo[] = "logo";
const char _name[] = "name";
const char _subTopicIds[] = "subTopicIds";
const char _subjectCode[] = "subjectCode";
const char _subtitle[] = "subtitle";
const char _topicIds[] = "topicIds";
const char _performances[] = "performances";
const char _eventId[] = "eventId";
const char _prices[] = "prices";
const char _amount[] = "amount";
const char _audienceSubCategoryId[] = "audienceSubCategoryId";
const char _seatCategoryId[] = "seatCategoryId";
const char _seatCategories[] = "seatCategories";
const char _areas[] = "areas";
const char _areaId[] = "areaId";
const char _blockIds[] = "blockIds";
const char _seatMapImage[] = "seatMapImage";
const char _start[] = "start";
const char _venueCode[] = "venueCode";
const char _seatCategoryNames[] = "seatCategoryNames";
const char _subTopicNames[] = "subTopicNames";
const char _subjectNames[] = "subjectNames";
const char _topicNames[] = "topicNames";
const char _topicSubTopics[] = "topicSubTopics";
const char _venueNames[] = "venueNames";

} // namespace

int main(int argc, char *argv[]) {
  using js_schema = ctrj::obj<
      ctrj::fld<_areaNames, ctrj::dyn_obj<ctrj::str>>,
      ctrj::fld<_audienceSubCategoryNames, ctrj::dyn_obj<ctrj::str>>,
      ctrj::fld<_blockNames, ctrj::dyn_obj<ctrj::str>>,
      ctrj::fld<_events, ctrj::dyn_obj<ctrj::obj<
          ctrj::fld<_description, ctrj::nul<ctrj::str>>,
          ctrj::fld<_id, ctrj::u64>,
          ctrj::fld<_logo, ctrj::nul<ctrj::str>>,
          ctrj::fld<_name, ctrj::nul<ctrj::str>>,
          ctrj::fld<_subTopicIds, ctrj::arr<ctrj::u64>>,
          ctrj::fld<_subjectCode, ctrj::nul<ctrj::u64>>,
          ctrj::fld<_subtitle, ctrj::nul<ctrj::str>>,
          ctrj::fld<_topicIds, ctrj::arr<ctrj::u64>>
      >>>,
      ctrj::fld<_performances, ctrj::arr<ctrj::obj<
          ctrj::fld<_eventId, ctrj::u64>,
          ctrj::fld<_id, ctrj::u64>,
          ctrj::fld<_logo, ctrj::nul<ctrj::str>>,
          ctrj::fld<_name, ctrj::nul<ctrj::str>>,
          ctrj::fld<_prices, ctrj::arr<ctrj::obj<
              ctrj::fld<_amount, ctrj::u64>,
              ctrj::fld<_audienceSubCategoryId, ctrj::u64>,
              ctrj::fld<_seatCategoryId, ctrj::u64>
          >>>,
          ctrj::fld<_seatCategories, ctrj::arr<ctrj::obj<
              ctrj::fld<_areas, ctrj::arr<ctrj::obj<
                  ctrj::fld<_areaId, ctrj::u64>,
                  ctrj::fld<_blockIds, ctrj::arr<ctrj::u64>>
              >>>,
              ctrj::fld<_seatCategoryId, ctrj::u64>
          >>>,
          ctrj::fld<_seatMapImage, ctrj::nul<ctrj::str>>,
          ctrj::fld<_start, ctrj::u64>,
          ctrj::fld<_venueCode, ctrj::str>
      >>>,
      ctrj::fld<_seatCategoryNames, ctrj::dyn_obj<ctrj::str>>,
      ctrj::fld<_subTopicNames, ctrj::dyn_obj<ctrj::str>>,
      ctrj::fld<_subjectNames, ctrj::dyn_obj<ctrj::str>>,
      ctrj::fld<_topicNames, ctrj::dyn_obj<ctrj::str>>,
      ctrj::fld<_topicSubTopics, ctrj::dyn_obj<ctrj::arr<ctrj::u64>>>,
      ctrj::fld<_venueNames, ctrj::dyn_obj<ctrj::str>>
  >;

  int N = std::stoi(argv[1]);

  std::ifstream is{std::string(argv[2])};
  std::string content{};
  content.assign(std::istreambuf_iterator<char>(is),
                 std::istreambuf_iterator<char>());
  auto content_c_str = content.c_str();
  ctrj::val<js_schema> js{};

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < N; i++) {
    rapidjson::StringStream ss{content_c_str};
    rapidjson::Reader reader{};
    ctrj::reader<js_schema, rapidjson::BaseReaderHandler, rapidjson::UTF8<>>
        handler{js};
    reader.Parse(ss, handler);
    if (reader.HasParseError())
      std::cout << "ERROR" << std::endl;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << elapsed.count() / N << " s/OP" << std::endl;
  std::cout << (double) (content.length() * N) / elapsed.count() / 1024 / 1024
            << " MiB/s" << std::endl;

  return 0;
}