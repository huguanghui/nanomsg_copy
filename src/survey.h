#ifndef SURVEY_H_INCLUDED
#define SURVEY_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define NN_PROTO_SURVEY (6)

#define NN_SURVEYOR (NN_PROTO_SURVEY * 16 + 2)
#define NN_RESPONDENT (NN_PROTO_SURVEY * 16 + 3)

#define NN_SURVEYOR_DEADLINE (1)

#ifdef __cplusplus
}
#endif

#endif // !SURVEY_H_INCLUDED
