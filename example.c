#define CONFIG_SM_FSM 1

#include "sm.h"
#include <stdio.h>
#include <stdlib.h>

#define UNUSED_ARG(x) ((x) = (x))
#define FsmBool unsigned char
#define FsmTrue 1
#define FsmFalse 0
#define BUF_LEN_MAX 1024

typedef FsmBool (*p_event)(void);

enum sm_user_sig
{
    SM_ORIGIN_SIG = SM_USER_SIG + 1,
    SM_INPUT_SIG,
    SM_CHAR_SIG,
    SM_SLASH_SIG,
    SM_STAR_SIG,
    SM_COMMENT_SIG,
    SM_DONE_SIG,
    SM_SIG_NUM
};

sm_t fsm_me;
sm_sig_t usr_sig = SM_ORIGIN_SIG;
unsigned char usr_buffer[BUF_LEN_MAX] = {0u};
unsigned int buffer_len = 0u;
unsigned int len_temp = 0u;
unsigned int comment_start_len = 0;

sm_ret_t fsm_init_handler(sm_t *fsm, sm_event_t const *e);
sm_ret_t input_handler(sm_t *fsm, sm_event_t const *e);
sm_ret_t char_handler(sm_t *fsm, sm_event_t const *e);
sm_ret_t slash_handler(sm_t *fsm, sm_event_t const *e);
sm_ret_t star_handler(sm_t *fsm, sm_event_t const *e);
sm_ret_t comment_handler(sm_t *fsm, sm_event_t const *e);

FsmBool input_event(void);
FsmBool char_event(void);
FsmBool slash_event(void);
FsmBool star_event(void);
FsmBool comment_event(void);
FsmBool done_event(void);

sm_event_t sm_user_event[] =
    {
        {SM_ORIGIN_SIG, SM_NULL},
        {SM_INPUT_SIG, input_event},
        {SM_CHAR_SIG, char_event},
        {SM_SLASH_SIG, slash_event},
        {SM_STAR_SIG, star_event},
        {SM_COMMENT_SIG, comment_event},
        {SM_DONE_SIG, done_event}};

FsmBool input_event(void)
{
    int c;

    while ((c = getchar()) != EOF)
    {
        usr_buffer[buffer_len] = c;

        if (buffer_len < BUF_LEN_MAX)
        {
            buffer_len++;
        }
        else
        {
            printf("Too much input, the maximum support is 1024 characters!!!");
        }
    }

    printf("\nbuffer_len is %d", buffer_len);
    printf("\n----Input Done----\n");
    usr_sig = SM_CHAR_SIG;
    return FsmTrue;
}

FsmBool char_event(void)
{
    FsmBool result = FsmFalse;

    for (; len_temp < buffer_len; len_temp++)
    {
        if (usr_buffer[len_temp] == '/')
        {
            usr_sig = SM_SLASH_SIG;
            result = FsmTrue;
            break;
        }
    }

    if (len_temp == buffer_len)
    {
        printf("Input string: ");
        for (int i = 0; i < buffer_len; i++)
            printf("%c", usr_buffer[i]);
        printf("\nNO comment!!!\n");
        exit(0);
    }

    return result;
}

FsmBool slash_event(void)
{
    FsmBool result = FsmFalse;
    static unsigned char comment_start_flag = 0u;

    for (len_temp += 1; len_temp < buffer_len; len_temp++)
    {
        if (usr_buffer[len_temp] == '*')
        {
            if (comment_start_flag == 0u)
            {
                comment_start_flag = 1u;
                printf("len_temp : %d \n", len_temp);
                comment_start_len = len_temp;
            }
            len_temp += 1;
            usr_sig = SM_COMMENT_SIG;
            result = FsmTrue;
            break;
        }
        if (usr_buffer[len_temp] == '/')
        {
            break;
        }
        else
        {
            len_temp += 1;
            usr_sig = SM_CHAR_SIG;
            result = FsmTrue;
            break;
        }
    }

    if (len_temp == buffer_len)
    {
        printf("Input string: ");
        for (int i = 0; i < buffer_len; i++)
            printf("%c", usr_buffer[i]);
        printf("\nNO comment!!!\n");
        exit(0);
    }

    return result;
}

FsmBool star_event(void)
{
    FsmBool result = FsmFalse;

    for (len_temp += 1; len_temp < buffer_len; len_temp++)
    {
        if (usr_buffer[len_temp] == '/')
        {
            printf("Comment string: /*");
            for (comment_start_len = comment_start_len + 1; comment_start_len < buffer_len; comment_start_len++)
                printf("%c", usr_buffer[comment_start_len]);
            printf("\nDone..... \n");
            exit(0);
            break;
        }
        if (usr_buffer[len_temp] == '*')
        {
            break;
        }
        else
        {
            usr_sig = SM_COMMENT_SIG;
            result = FsmTrue;
            break;
        }
    }

    if (len_temp == buffer_len)
    {
        printf("Input string: ");
        for (int i = 0; i < buffer_len; i++)
            printf("%c", usr_buffer[i]);
        printf("\nNO comment!!!\n");
        exit(0);
    }

    return result;
}

FsmBool comment_event(void)
{
    FsmBool result = FsmFalse;

    for (; len_temp < buffer_len; len_temp++)
    {
        if (usr_buffer[len_temp] == '*')
        {
            // printf("%c", usr_buffer[len_temp]);
            usr_sig = SM_STAR_SIG;
            result = FsmTrue;
            break;
        }
    }

    if (len_temp == buffer_len)
    {
        printf("Input string: ");
        for (int i = 0; i < buffer_len; i++)
            printf("%c", usr_buffer[i]);
        printf("\nNO comment!!!\n");
        exit(0);
    }

    return result;
}

FsmBool done_event(void)
{
    FsmBool result = FsmFalse;
    return result;
}

sm_ret_t fsm_init_handler(sm_t *fsm, sm_event_t const *e)
{
    UNUSED_ARG(e);

    printf("FSM example init.\n");

    usr_sig = SM_INPUT_SIG;

    return SM_TRAN(fsm, input_handler);
}

sm_ret_t input_handler(sm_t *fsm, sm_event_t const *e)
{
    switch (e->sig)
    {
    case SM_EXIT_SIG:
        printf("Exit FSM input handler\r\n");
        break;
    case SM_ENTRY_SIG:
        printf("Enter FSM input handler.\n");
        printf("Plesae input string: \n");
        break;
    case SM_INPUT_SIG:
        if (e->event != SM_NULL)
            if (((p_event)(e->event))())
                return SM_TRAN(fsm, char_handler);
    default:
        break;
    }

    return SM_HANDLED();
}

sm_ret_t char_handler(sm_t *fsm, sm_event_t const *e)
{
    switch (e->sig)
    {
    case SM_EXIT_SIG:
        printf("Exit FSM char handler\n");
        break;
    case SM_ENTRY_SIG:
        printf("Enter FSM char handler\n");
        break;
    case SM_CHAR_SIG:
        if (e->event != SM_NULL)
        {
            if (((p_event)(e->event))())
                return SM_TRAN(fsm, slash_handler);
        }
        else
        {
            printf("Event is Null!!!");
            exit(0);
        }
    default:
        break;
    }

    return SM_HANDLED();
}

sm_ret_t slash_handler(sm_t *fsm, sm_event_t const *e)
{
    switch (e->sig)
    {
    case SM_EXIT_SIG:
        printf("Exit FSM slash handler\n");
        break;
    case SM_ENTRY_SIG:
        printf("Enter FSM slash handler.\n");
        break;
    case SM_SLASH_SIG:
        if (e->event != SM_NULL)
        {
            if (((p_event)(e->event))())
            {
                if (usr_sig == SM_COMMENT_SIG)
                {
                    return SM_TRAN(fsm, comment_handler);
                }
                else if (usr_sig == SM_CHAR_SIG)
                {
                    return SM_TRAN(fsm, char_handler);
                }
            }
        }
        else
        {
            printf("Event is Null!!!");
            exit(0);
        }
    default:
        break;
    }

    return SM_HANDLED();
}

sm_ret_t comment_handler(sm_t *fsm, sm_event_t const *e)
{
    switch (e->sig)
    {
    case SM_EXIT_SIG:
        printf("Exit FSM comment handler\n");
        break;
    case SM_ENTRY_SIG:
        printf("Enter FSM comment handler.\n");
        break;
    default:
        if (e->event != SM_NULL)
        {
            if (((p_event)(e->event))())
            {
                if (usr_sig == SM_STAR_SIG)
                {
                    return SM_TRAN(fsm, star_handler);
                }
            }
        }
        else
        {
            printf("Error state!!!");
            exit(0);
        }

        break;
    }

    return SM_HANDLED();
}

sm_ret_t star_handler(sm_t *fsm, sm_event_t const *e)
{
    switch (e->sig)
    {
    case SM_EXIT_SIG:
        printf("Exit FSM star handler\n");
        break;
    case SM_ENTRY_SIG:
        printf("Enter FSM star handler.\n");
        break;
    default:
        if (e->event != SM_NULL)
        {
            if (((p_event)(e->event))())
            {
                if (usr_sig == SM_COMMENT_SIG)
                {
                    return SM_TRAN(fsm, comment_handler);
                }
            }
        }
        else
        {
            printf("Error state!!!");
            exit(0);
        }

        break;
    }

    return SM_HANDLED();
}

int main(void)
{
    fsm_ctor(&fsm_me, fsm_init_handler);
    (void)fsm_init(&fsm_me, &sm_user_event[usr_sig]);

    while (1)
    {
        fsm_dispatch(&fsm_me, &sm_user_event[usr_sig]);
    }

    return 0;
}
