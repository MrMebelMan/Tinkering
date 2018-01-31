#!/bin/bash
# blackjack.sh

clear

declare DEBUG="OFF" RET="" PLAYER_SKIP="" BOT_SKIP=""
declare -ai DECK PLAYER_DECK BOT_DECK
declare -i GAME_GOAL=42 PLAYER_VALUE=0 BOT_VALUE=0 BOT_CAUTION_LEVEL=$(((RANDOM % 12)+1))
declare -i IN_PROGRESS=0 END=1 TAKE=0 NO_TAKE=1 BOT_DECISION=$NO_TAKE
declare -i GAME_STATE=$IN_PROGRESS DECK_CNT=0 DECK_SIZE=0 PLAYER_DECK_SIZE=0 BOT_DECK_SIZE=0
declare -A CARD_NAMES=( ['1']='Ace' ['2']='2' ['3']='3' ['4']='4' ['5']='5' ['6']='6'
                        ['7']='7' ['8']='8' ['9']='9' ['10']='Jack' ['11']='Queen' ['12']='King' )
declare NAMES=( "Itzhak Weissman" "Lily Shots" "Serah Fields" "Sid Zilla" 
                "Max Gingerbeard" "John Sweeps" "Chocola Nakimato" "Hortensia Harrison" )
declare BOT_NAME=${NAMES[$((RANDOM%${#NAMES[@]}))]}

echo "Game goal: $GAME_GOAL."
echo "You are playing against $BOT_NAME."
if [ $BOT_CAUTION_LEVEL -gt 9 ]; then
  echo "$BOT_NAME looks very nervous."
elif [ \( $BOT_CAUTION_LEVEL -gt 6 \) -a \( $BOT_CAUTION_LEVEL -le 9 \) ]; then
  echo "$BOT_NAME seems to be very cautious."
elif [ \( $BOT_CAUTION_LEVEL -gt 3 \) -a \( $BOT_CAUTION_LEVEL -le 6 \) ]; then
  echo "$BOT_NAME looks confident."
elif [ \( $BOT_CAUTION_LEVEL -gt 0 \) -a \( $BOT_CAUTION_LEVEL -le 3 \) ];then
  echo "$BOT_NAME is feeling risky and adventurous!"
fi
echo

function shuffle_deck ()
{
  TIMES=$((RANDOM%10+1))
  echo "The dealer shuffles the deck $TIMES times without even looking what they're doing. Cool!"
  echo
  SHUF_CNT=$TIMES
  while [ $((--SHUF_CNT)) -ge 0 ]
  do
    for i in $(seq 0 $((DECK_SIZE-1)))
    do
      POS1=$((RANDOM % DECK_SIZE))
      POS2=$((RANDOM % DECK_SIZE))
      [ $POS1 -eq $POS2 ] && continue
      TMP=${DECK[$POS1]}
      DECK[$POS1]=${DECK[$POS2]}
      DECK[$POS2]=$TMP
    done
  done
}

function pop ()
{
  DECK[$((DECK_SIZE-1))]=-1
  ((--DECK_SIZE))
}

function fill_deck ()
{
  for CARD in {1..12}
  do
    for SUIT in {1..4}
    do
      DECK[$((DECK_CNT++))]=CARD
    done
  done
  DECK_SIZE=${#DECK[@]}
}

function show_deck ()
{
  for i in ${!DECK[*]}
  do
    [ \( ${DECK[$i]} -eq 1 \) -o \( ${DECK[$i]} -ge 10 \) ] && echo ${CARD_NAMES[${DECK[$i]}]} || echo ${DECK[$i]}
  done
}

function get_card_name ()
{
  if [ \( $1 -eq 1 \) -o \( $1 -ge 10 \) ]; then
    RET=${CARD_NAMES[$1]}
  else
    RET=$1
  fi
}

function player_hand_push ()
{
  PLAYER_DECK[$((PLAYER_DECK_SIZE++))]=$1
  [ \( $1 -eq 1 \) -a \( $((PLAYER_VALUE + 11)) -le $GAME_GOAL \) ] && PLAYER_VALUE=$((PLAYER_VALUE + 11)) || \
    PLAYER_VALUE=$((PLAYER_VALUE + $1))
  [ $PLAYER_VALUE -gt $GAME_GOAL ] && echo "Oh no! You have $PLAYER_VALUE points. $BOT_NAME wins!" && GAME_STATE=$END
}

function bot_hand_push ()
{
  BOT_DECK[$((BOT_DECK_SIZE++))]=$1
  [ \( $1 -eq 1 \) -a \( $((BOT_VALUE + 11)) -le $GAME_GOAL \) ] && BOT_VALUE=$((BOT_VALUE + 10)) || \
    BOT_VALUE=$((BOT_VALUE+$1))
  [ $BOT_VALUE -gt $GAME_GOAL ] && echo && echo "Oh YES! $BOT_NAME took too much! He has $BOT_VALUE points. You win!" && GAME_STATE=$END
}

function deal_player ()
{
  TOP_CARD=${DECK[$((DECK_SIZE-1))]}; pop
  get_card_name $TOP_CARD
  echo "You received a card: $RET "
  player_hand_push $TOP_CARD
}

function deal_bot ()
{
  TOP_CARD=${DECK[$((DECK_SIZE-1))]}; pop
  get_card_name $TOP_CARD
  echo -n "The Dealer deals $BOT_NAME a card."
  bot_hand_push $TOP_CARD
  if [ $BOT_VALUE -eq $GAME_GOAL ]; then
    echo " $BOT_NAME grins and rubs one's hands."
  elif [ \( $BOT_VALUE -gt $((GAME_GOAL - (GAME_GOAL/8) )) \) -a \( $BOT_VALUE -lt $GAME_GOAL \) ]; then
    echo " $BOT_NAME smiles."
  fi
  echo
}

function deal_starting_cards ()
{
  deal_player
  deal_player
  deal_bot
  deal_bot
}

function bot_decide ()
{
  [ "$DEBUG" = "ON" ] && echo "DEBUG: \$BOT_VALUE = $BOT_VALUE, BOT_CAUTION_LEVEL = $BOT_CAUTION_LEVEL"
  [ \( $BOT_VALUE -gt $(( GAME_GOAL - (BOT_CAUTION_LEVEL/2) )) \) -a \( $BOT_VALUE -ne $GAME_GOAL \) ] && echo -n 'After some hesitation, '
  if [ $BOT_VALUE -gt $((GAME_GOAL - BOT_CAUTION_LEVEL)) ]; then
    BOT_DECISION=$NO_TAKE
    echo -n "$BOT_NAME decides to skip their turn"
    [ $BOT_VALUE -eq $GAME_GOAL ] && echo " and smiles." || echo "."
    BOT_SKIP="YES"
  else
    BOT_DECISION=$TAKE
    echo "$BOT_NAME decides to take another card."
  fi
}

function check_results ()
{
  if [ $PLAYER_VALUE -gt $BOT_VALUE ]; then
    echo "Congratulations! You win!"
    echo "You: $PLAYER_VALUE points."
    echo "$BOT_NAME: $BOT_VALUE points."
  elif [ $BOT_VALUE -gt $PLAYER_VALUE ]; then
    echo "Oh no, you lose!"
    echo "You: $PLAYER_VALUE points"
    echo "$BOT_NAME: $BOT_VALUE points."
  elif [ $PLAYER_VALUE -eq $BOT_VALUE ]; then
    echo "Draw! You both have $PLAYER_VALUE points!"
  fi
  GAME_STATE=$END
}

function show_hand ()
{
  echo -n "Your hand: "
  for i in $(seq 0 $((PLAYER_DECK_SIZE-1)))
  do
    echo -n "${CARD_NAMES[${PLAYER_DECK[$i]}]}, "
  done | sed 's/, $//'
  echo
  echo
}

function run_game_cycle ()
{
  read dummy
  
  while [ $GAME_STATE != $END ]
  do
    clear
    while :
    do
      show_hand
      printf "1) Take another card.\n2) Skip this turn.\n3) Quit.\n\n"
      read REPLY
      if [ "$REPLY" = 1 ]; then
        deal_player
        break
      elif [ "$REPLY" = 2 ]; then
        PLAYER_SKIP="YES"
        break
      elif [ "$REPLY" = 3 ]; then
        GAME_STATE=$END;
        break
      else
        clear
      fi
    done
  
    [ $GAME_STATE = $END ] && break
    bot_decide
    [ $BOT_DECISION = $TAKE ] && deal_bot
    [ \( "$PLAYER_SKIP" = "YES" \) -a \( "$BOT_SKIP" = "YES" \) ] && check_results
    read dummy
  done
}

# Main

fill_deck
shuffle_deck
deal_starting_cards
run_game_cycle

exit 0

