<template>
  <div :class="getClass()">

    <div v-if="state === 'thumbnails'">
      <h1 class="overviewTitle">{{recipe.title}}</h1>
      <div class="overviewMeta">
        <p>{{recipe.difficulty}}</p>
        <p>{{recipe.time}} minutes</p>
      </div>
    </div>

    <div class="recipeInner" v-if="state === 'ingredients' && selected">
      <h1 class="overviewTitle">{{recipe.title}}</h1>
      <div class="overviewMeta">
        <p>{{recipe.time}} minutes</p>
      </div>
      <div id="ingredients">
        Ingredients
        <div id="ingredientsList">
          <div v-for="(ingredient, idx) in this.recipe.ingredients" class="ingredient">
            {{getIngredientText(ingredient)}}
          </div>
        </div>
      </div>
    </div>

    <div class="inRecipe" v-show="state === 'steps' && selected">
      <div id="steps">
        <StepDisplay :steps="recipe.steps" :stepIdx="selectedStep"/>
      </div>
      <div id="weighers">
        <WeightDisplay :eventBus="eventBus" v-if="selected && stepHasWeight()" :requiredWeight="getStepRequiredWeight()"
                       :name="getStepWeightName()"/>
      </div>
      <div id="timers">
        <Timer v-for="(timer, idx) in this.activeTimers" :key="idx" :running="timer.running" :name="timer.name"
               :startTime="timer.startTime"/>
      </div>
    </div>

    <div v-if="state === 'instructions'">
    	Show me how to do it.
    </div>
  </div>

</template>

<script>

  import StepDisplay from './StepDisplay'
  import Timer from './Timer'
  import WeightDisplay from './WeightDisplay'
  import ChopInstructions from './ChopInstructions'

  export default {
    name: 'recipeOverview',
    props: ['recipe', 'state', 'selected', "eventBus", "portions"],
    data() {
      return {
        selectedStep: 0,
        activeTimers: [],
      }
    },
    created() {
      this.eventBus.$on("pressed", this.handlePress);
    },

    components: {
      StepDisplay,
      Timer,
      WeightDisplay
    },

    methods: {
      handlePress(dir) {
        if (this.state ==='steps' && this.selected) {
          if (dir === 'right') {
            this.nextStep();
          } else if (dir === 'left') {
            this.prevStep();
          } else if (dir === 'down' && this.hasTimeAtCurrentStep()) {
            this.startTimer();
          } else if (dir === 'down' && this.hasInsructions() && this.state==='steps') {
            this.showInstruction();
          }else if (dir === 'up')  {
            this.$parent.upPressed();
          }
        }else if(this.state==='instructions'){
        	if(dir === 'up'){
        		this.$parent.upPressed();
        	}
        }
      },
      nextStep: function () {
        if (this.selectedStep < this.recipe.steps.length - 1) {
          this.selectedStep++;
        }
      },
      prevStep: function () {
        if (this.selectedStep > 0) {
          this.selectedStep--;
        }
      },
      startTimer() {
        if (this.hasTimeAtCurrentStep()) {
          console.log("starting timer for " + this.getStepTime() + "mins for " + this.getTimerName());
          let timerName = this.getTimerName();

          if (this.timerSet(timerName)) {
            console.log("timer already set for " + timerName);
            return; //since timer is already set
          }

          let timer = {
            startTime: this.getStepTime(),
            name: timerName,
            running: true,
          };

          this.activeTimers.push(timer)
        }
      },
      showInstruction() {
      	if (this.hasInsructions()){
      		console.log("I'd want to show some pretty GIFs");
      		this.$parent.showInstructions();
      	}
      },
      removeWithDelay(timerName) {
        let self = this;
        let pause = 15000;
        window.setTimeout(function () {
          self.removeTimer(timerName)
        }, pause)
      },
      removeTimer(timerName) {
        let toRemove = -1;
        for (let i = 0; i < this.activeTimers.length; i++) {
          if (this.activeTimers[i].name === timerName) {
            toRemove = i;
            break;
          }
        }

        if (toRemove !== -1) {
          this.activeTimers.splice(toRemove, 1)
        }

      },
      hasTimeAtCurrentStep() {
        return this.currentStep().time !== undefined;
      },

      hasInsructions(){
      	return this.currentStep().extra !== undefined;
      },

      getStepTime() {
        return this.currentStep().time;
      },
      getTimerName() {
        const tn = this.currentStep().timerName;
        if (tn !== undefined) return tn;
        return "Timer";
      },
      timerSet(name) {
        for (let timer of this.activeTimers) {
          if (timer.name === name) {
            return true;
          }
        }
        return false;
      },
      getClass: function () {
        let divClass = "recipeOverview";
        if (this.state === 'thumbnails' && this.selected) {
          divClass += " hovered"
        }

        if (['ingredients', 'steps'].includes(this.state) && this.selected) {
          divClass += " opened"
        } else if (this.activeTimers.length > 0) {
          this.activeTimers = []; //clear timers when changing recipes
        }
        return divClass
      },
      getNiceFrac(value) {
        let fracPart = value % 1;
        let wholePart = value - fracPart;
        let wholeStr = "";
        if (wholePart > 0) {
          wholeStr = wholePart + " ";
        }
        if (fracPart > 0.31 && fracPart < 0.34) {
          return wholeStr + "1/2"
        } else if (fracPart === 0.5) {
          return wholeStr + "1/3"
        } else if (fracPart > 0.65 && fracPart < 0.67) {
          return wholeStr + "2/3"
        } else if (fracPart === 0.75) {
          return wholeStr + "3/4";
        }
        return value;
      },

      getIngredientText(ingredient) {
        //this code isn't nice, sorry
        let text = "";
        if (ingredient.units.length === 2) {
          //
          if (ingredient.units[0] === "each") {
            let quantity = ingredient.quantity[0] / this.recipe.serving * this.portions; //Make ingredient scale to portions
            text += this.getNiceFrac(quantity) + " ";
            text += ingredient.name;
            if (quantity > 1 && ingredient.name[ingredient.name.length - 1] !== "s") {
              text += "s" //pluralise names
            }
            text += " (" + (ingredient.quantity[1] / this.recipe.serving * this.portions) + " " + ingredient.units[1] + ") "
          }
        } else {
          if (ingredient.units[0] === "each") {
            let quantity = ingredient.quantity[0] / this.recipe.serving * this.portions;
            text += this.getNiceFrac(quantity) + " ";
            text += ingredient.name
          } else if (ingredient.units[0] === "to taste") {
            text += ingredient.name + " ";
            text += ingredient.units[0]

          }
          else {
            let quantity = ingredient.quantity[0] / this.recipe.serving * this.portions;
            text += quantity + " " + ingredient.units[0] + " ";
            text += ingredient.name
          }
        }
        return text
      },
      stepHasWeight() {
        return this.currentStep().weighing === true;
      },
      currentStep() {
        return this.recipe.steps[this.selectedStep];
      },

      getStepRequiredWeight() {
        let ingred = this.currentStep().ingredientName;

        if (ingred === undefined) {
          console.log("error undefined ingredient");
          return 0;
        }
        // debugger

        return this.recipe.ingredients[ingred].quantity[0] * this.portions;
      },
      getStepWeightName() {
        return this.currentStep().ingredientName
      }
    }
  }
</script>


<style scoped>
  h1.overviewTitle {
    font-size: 32px;
  }

  .overviewMeta {
    font-family: 'Roboto', Helvetica, Arial, sans-serif;
    margin-top: 15px;
  }

  .overviewMeta p {
    font-size: 22px;
    margin: 5px 0;
  }

  .recipeOverview {
    max-width: 33%;
    overflow: hidden;
    margin: 10px;
    border: 1px black solid;
    height: 80%;
    width: 100%;
    transition: background 0.3s, max-height max-width 1s;
    display: flex;
    flex-direction: row;
    align-items: center;
    padding: 10px;
  }

  .recipeOverview.hovered {
    background: #fce8ff;
    transition: background 0.3s;
  }

  .recipeOverview.opened {
    background: inherit;
    margin: 0;
    transition: 1s;
    border: 0;
    height: 100%;
    width: 100%;
    max-height: 100%;
    max-width: 100%;
  }

  .ingredient {
    width: 50%;
    display: inline-block;
    float: left;
  }

  #ingredientsList {
    clear: both;
    overflow: hidden;
  }

  .recipeInner {
    background: #fce8ff;
    width: 740px;
    margin: 10px;
    overflow: hidden;
    padding: 20px 0;
  }

  .timers {
    background: blue;
  }

  .inRecipe {
    width: 100%;
  }

</style>
