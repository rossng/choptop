<template>
  <div class="weightDisplay">
    <div class="weightCropper">
      <vue-circle
        ref="circ"
        :progress="0"
        :size="100"
        :reverse="false"
        :fill='{gradient: ["#fdd250", "#3aeabb","#3aeabb", "red"], gradientAngle: Math.PI / 4}'
        line-cap="round"
        show-percent="false"
        :animation-start-value="0"
        :start-angle="(6/2)*Math.PI"
        insert-mode="append"
        :animation="{ duration: 200, easing: 'easeOutBounce' }"
        :thickness="10"
        :show-percent="false">
      </vue-circle>
    </div>
    <div class="weightText">
      <span class="weightValue"><AnimatedInt :value="taredWeight()"/>g</span>
      <span class="weigherName">{{name.charAt(0).toUpperCase() + name.slice(1)}}</span>
      <span class="totalWeight">{{requiredWeight}}g</span>
    </div>
  </div>
</template>

<script>
  import VueCircle from 'vue2-circle-progress';
  import AnimatedInt from './AnimatedInteger'

  export default {
    name: 'weightDisplay',
    props: ['eventBus', "requiredWeight", "name"],
    components: {
      AnimatedInt,
      VueCircle
    },
    created() {
      this.eventBus.$on("weight", this.handleNewWeight.bind(this));
      this.eventBus.$on("tare", this.handleTare);
      this.eventBus.$on("pressed", this.handlePress);
    },
    methods: {
      handlePress(dir) {
        if (this.state === 'steps' && this.selected) {
          if (dir === 'down') {
            this.handleTare();
          }
        }
      },
      handleNewWeight(weight) {
        console.log("new weight" + weight)
        this.currentWeight = weight;
        this.$refs.circ.updateProgress(this.weightCompleteness())
      },
      handleTare() {
        this.tareValue = this.currentWeight;
      },
      taredWeight() {
        return this.round(this.currentWeight - this.tareValue);
      },
      round(weight) {
        // Rounds the weight to the closest 5 grams. Only rounds the displayed value so the error wouldn't build up.
        return Math.round(weight / 5) * 5;
      },
      weightCompleteness() {
        return (this.taredWeight() / this.requiredWeight) * 100 / 2;
      }
    },
    data() {
      return {
        currentWeight: 0,
        tareValue: 0,
        circle: null
      }
    }

  }
</script>


<style scoped>
  .weightDisplay {
    text-align: center;
    display: inline-block;
    position: relative;
  }

  .weightCropper {
    overflow: hidden;
    height: 50px;
  }

  .weightText {
    position: absolute;
    left: 0px;
    top: 16px;
    width: 100px;
  }

  .weightText span.weightValue, .weightText span.weigherName, .weightText span.totalWeight {
    display: block;
  }

</style>
