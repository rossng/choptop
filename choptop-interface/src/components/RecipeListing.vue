<template>
  <div class="recipeOverviewListingOuter">
    <div :class="{arr:true, upArr:true}">
      <icon name="angle-up" scale="4"></icon>
    </div>
    <div class="recipeOverviewListing">
      <div v-if="currentState === 'thumbnails'" :class="{arr:true, leftarr:true}">
        <icon name="angle-left" scale="4"></icon>
      </div>
      <RecipeOverview v-for="(rec, index) in recipes" :key="rec.name" :eventBus="eventBus"
                      v-if="currentState === 'thumbnails' || chosenRecipeIndex === index" :recipe="rec"
                      :state="currentState"
                      :selected="chosenRecipeIndex === index"
                      :portions="portionCount"/>
      <div v-if="currentState === 'thumbnails'" :class="{rightArr:true, arr:true}">
        <icon name="angle-right" scale="4"></icon>
      </div>
    </div>
    <div v-if="currentState !== 'steps'" :class="{arr:true, downArr:true}">
      <icon name="angle-down" scale="4"></icon>
    </div>
  </div>
</template>

<script>
  import RecipeOverview from './RecipeOverview'


  export default {
    name: 'recipeListing',
    components: {
      RecipeOverview
    },

    created() {
      this.eventBus.$on("pressed", this.handlePress);
    },

    methods: {
      handlePress(dir) {
        if (this.currentState === 'thumbnails') {
          if (dir === 'right') {
            this.nextRecipe();
          } else if (dir === 'left') {
            this.previousRecipe();
          } else if (dir === 'down') {
            this.currentState = 'ingredients';
          } else if (dir === 'up') {
            this.$parent.upPressed();
          }
        } else if (this.currentState === 'ingredients') {
          if (dir === 'down') {
            this.currentState = 'steps';
          } else if (dir === 'up') {
            this.currentState = 'thumbnails';
          }
        } else if (this.currentState === 'steps') {
        }
      },
      upPressed() {
        this.currentState = 'ingredients'
      },
      nextRecipe() {
        this.chosenRecipeIndex = (this.chosenRecipeIndex + 1) % this.recipes.length
      },
      previousRecipe() {
        this.chosenRecipeIndex = (this.chosenRecipeIndex - 1 + this.recipes.length) % this.recipes.length //correct modulo
      }
    },

    props: ['recipes', 'eventBus', 'portionCount'],
    data() {
      return {
        chosenRecipeIndex: 0,
        currentState: 'thumbnails', // one of ['thumbnails', 'ingredients', 'steps']
      }
    }
  }
</script>


<style scoped>
  h1 {

  }

  .recipeOverviewListing {
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: center;
    height: 100%;
    width: 100%;
  }

  .recipeOverviewListingOuter {
    display: flex;
    flex-direction: column;
    justify-content: space-between;
    align-items: center;
    height: 100%;
  }

  .arr {
    font-size: 100px;
    opacity: 1;
    transition: 0.5s;
    display: inline-flex;
    height: 35px;
  }

  .upArr svg {
    margin-top: -17px;

  }

  .downArr svg {
    margin-top: -21px;
  }

</style>
