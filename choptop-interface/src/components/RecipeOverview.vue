<template>
	<div :class="getClass()">
		<div v-if="!selected">
			<h1 class="overviewTitle">{{recipe.title}}</h1>
			<h2>Difficulty: {{recipe.difficulty}}</h2>
			<h2>Time: {{recipe.time}} minutes</h2>
			<!-- <icon class="fa fa-angle-down" aria-hidden="true"></icon> -->
			<icon name="angle-down"></icon>
		</div>
		<div class="recipeInner" v-if="selected && !stepsVisible">
			<h1>{{recipe.title}}</h1>
			<h2>Difficulty: {{recipe.difficulty}}</h2>
			<h2>Time: {{recipe.time}} minutes</h2>

			<div id="ingredients">
				Ingredients
				<div id="ingredientsList">
					<div v-for="(ingredient, idx) in this.recipe.ingredients" class="ingredient">
						{{getIngredientText(ingredient)}}
					</div>
				</div>
			</div>
		</div>	

		<div class="inRecipe" v-show="selected && stepsVisible">	
			<div id="steps">
				<StepDisplay :steps="recipe.steps" :stepIdx="selectedStep"/>
			</div>
			<div id="timers">
				<Timer v-for="(timer, idx) in this.activeTimers" :key="idx" :running="timer.running" :name="timer.name" :startTime="timer.startTime"/>
			</div>
		</div>

	</div>
</template>

<script>

	import StepDisplay from './StepDisplay'
	import Timer from './Timer'

	export default {
	  name: 'recipeOverview',
	  props:['recipe', 'hovered', 'selected', "eventBus", "portions"],
	  data () {
	    return {
	      selectedStep:0,
	      focussed: false,
	      stepsVisible:false,
	      activeTimers: [],
	    }
	  },
	  created(){
			this.eventBus.$on("pressed", this.handlePress);
	  },

	  components:{
	  	StepDisplay,
	  	Timer
	  },
	  
	  methods: {
	  	handlePress(dir){
			if (this.selected){
				if(dir == "right" && this.stepsVisible){
					console.log("right step")
					this.nextStep();
				}else if(dir =="left" && this.stepsVisible){
					this.prevStep();
				}else if(dir == "down"){
					console.log("steps visible")
					if(!this.stepsVisible){
						this.stepsVisible = true;
					}else{
						this.startTimer();
					}
				}else if(dir == "up"){
					if(this.stepsVisible){
						this.stepsVisible = false;
					}else{
						this.$parent.upPressed();
					}
				}
			}
		},
	  	nextStep: function(){
	  		if(this.selectedStep < this.recipe.steps.length - 1){
	  			this.selectedStep++;
	  		}
	  	},
	  	prevStep: function(){
	  		if(this.selectedStep > 0){
	  			this.selectedStep--;
	  		}
	  	},
	  	startTimer(){
	  		if(this.hasTimeAtCurrentStep()){
	  			console.log("starting timer for " + this.getStepTime() + "mins for " + this.getTimerName());
	  			var timerName = this.getTimerName();

	  			if(this.timerSet(timerName)){
	  				console.log("timer already set for " + timerName);
	  				return; //since timer is already set
	  			}

	  			var timer = {
	  				startTime: this.getStepTime(),
	  				name: timerName,
	  				running: true,
	  			}

	  			this.activeTimers.push(timer)
	  		}
	  	},
	  	hasTimeAtCurrentStep(){
		  		return this.recipe.steps[this.selectedStep].time != undefined;
		},

		getStepTime(){
			return this.recipe.steps[this.selectedStep].time;
		},
		getTimerName(){
			var tn = this.recipe.steps[this.selectedStep].timerName;
			if (tn != undefined) return tn;
			return "Timer";
		},
		timerSet(name){
			for (var timer of this.activeTimers){
				if(timer.name == name){
					return true;
				}
			}
			return false;
		},
	  	getClass : function(){
	  		var divClass = "recipeOverview"
	  		if(this.hovered && !this.selected){
	  			divClass += " hovered"
	  		}

	  		if(this.selected){
	  			divClass += " selected"
	  		}else if(this.activeTimers.length > 0){
	  			this.activeTimers = []; //clear timers when changing recipes
	  		}
	  		return divClass
	  	},
	  	getNiceFrac(value){
	  		var fracPart = value % 1;
	  		var wholePart = value  - fracPart
	  		var wholeStr = "";
	  		if(wholePart > 0){
	  			wholeStr = wholePart + " ";
	  		}
	  		if (fracPart > 0.31 && fracPart < 0.34){
	  			return wholeStr + "1/2"
	  		}else if(fracPart == 0.5){
	  			return wholeStr + "1/3"
	  		}else if(fracPart > 0.65 && fracPart < 0.67){
	  			return wholeStr + "2/3"
	  		}else if(fracPart == 0.75){
	  			return wholeStr + "3/4";
	  		}
	  		return value;
	  	},

	  	getIngredientText(ingredient){
	  		var logging = ingredient.name ==="a bunch of spring onions"

	  		//this code isn't nice, sorry
	  		var text = "";
	  		if(ingredient.units.length == 2){
	  			// 
	  			if(ingredient.units[0] == "each"){
	  				var quantity = ingredient.quantity[0]/this.recipe.serving * this.portions //Make ingredient scale to portions
	  				text += this.getNiceFrac(quantity) + " "
	  				text += ingredient.name
	  				if(quantity > 1 && ingredient.name[ingredient.name.length -1] != "s"){
	  					text += "s" //pluralise names
	  				}
	  				text += " ("+(ingredient.quantity[1]/this.recipe.serving * this.portions)+ " " + ingredient.units[1]+") "
	  			}
	  		}else {
	  			if (ingredient.units[0] == "each"){
	  				var quantity = ingredient.quantity[0]/this.recipe.serving * this.portions
	  				text += this.getNiceFrac(quantity) + " "
	  				text += ingredient.name
	  			}else if (ingredient.units[0] == "to taste"){
	  				text += ingredient.name + " "
	  				text += ingredient.units[0]
	  				
	  			}
	  			else{
	  				var quantity = ingredient.quantity[0]/this.recipe.serving * this.portions
	  				text += quantity + " " + ingredient.units[0]+" "
	  				text += ingredient.name
	  			}
	  		}
	  		return text
	  	},

	  // 	setPortions(ingredient){

			// ingredient.quantity = (ingredient.quantity/this.recipe.serving) * this.portions;
			// // return this.portions 
	  // 	}
	  }
	}
</script>



<style scoped>
	h1.overviewTitle{
		font-size:30px;
	}
	.recipeOverview{
		transition: max-height max-width 1s;
		max-width:33%;
		overflow: hidden;
		margin:10px;
		border:1px black solid;
		height:80%;
		width:100%;
		transition:background 0.3s;
		display: flex;
    	flex-direction: row;
    	align-items: center;
    	padding:10px;
	}

	.recipeOverview.hovered{
		background:#ffe6bf;
		transition:background 0.3s;
	}

	.recipeOverview.selected{
		background: inherit;
		margin:0;
		transition: 1s;
		border:0;
		
		width:100%;
		max-height:100%;
		max-width: 100%;
	}

	.ingredient{
		width:50%;
		display:inline-block;
		float:left;
	}

	#ingredientsList{
		clear:both;
		overflow:hidden;
	}

	.recipeInner{
		background: #68b4b2;
		width:740px;
		margin:10px;
		overflow: hidden
	}

	.timers{
		background:blue;
	}

	.inRecipe{
		width:100%;
	}
	
</style>
